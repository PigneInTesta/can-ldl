#include "can_sender.hpp"
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>
#include <unistd.h>
#include <thread>

void CAN_SenderThread(std::vector<FrequencyGroup>& groups,
                      const char* ip,
                      uint16_t port,
                      std::atomic<bool>& stop) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("[ERROR] Failed to create UDP socket\n");
        stop.store(true);
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // Initialize timing right before the loop
    auto start_time = std::chrono::steady_clock::now();
    for (auto& group : groups) {
        group.last_sent = start_time;
    }

    while(!stop.load()) {
        auto now = std::chrono::steady_clock::now();
        // Initialise very far in the future to ensure the first group is processed immediately
        auto next_deadline = std::chrono::steady_clock::time_point::max();

        for (auto& group : groups){
            auto interval_ms = std::chrono::milliseconds(1000 / group.getFrequency());
            auto due = group.last_sent + interval_ms;

            if (now >= due) {
                uint32_t ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

                static uint32_t send_errors = 0;
                int sent = sendto(sock, group.buildPacket(ts_ms), group.getPacketSize(), 0,
                                    (struct sockaddr*)&server_addr, sizeof(server_addr));
                if (sent < 0) {
                    send_errors++;
                    printf("[ERROR] sendto failed (%u times)\n", send_errors);
                } else {
                    printf("[UDP] %d Hz | ts=%u ms | %hu bytes\n",
                                group.getFrequency(), ts_ms, group.getPacketSize());
                }

                // Avoid drift by calculating the next deadline based on the last sent time
                group.last_sent += interval_ms;
                due = group.last_sent + interval_ms;
            }

            if (due < next_deadline) {
                next_deadline = due;
            }
        }

        // Sleep until the next packet is due
        std::this_thread::sleep_until(next_deadline);
    }

    printf("[INFO] Sender thread shutting down\n");
    close(sock);
}