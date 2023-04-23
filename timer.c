#include <stdio.h>
#include <time.h>

void wait_45_min(){
int remaining_time = 60; // 60 seconds countdown
    time_t start_time = time(NULL);

    while (remaining_time > 0) {
        
        printf("\rTime remaining: %d seconds", remaining_time);
        fflush(stdout);
        sleep(1);
        // Wait for 1 second
        time_t current_time;
        do {
            current_time = time(NULL);
        } while (current_time == start_time); // Wait for 1 second
        remaining_time--;
    }

    printf("\rTime's up!        \n"); // clear remaining time and print final message

}
int main() {
    
    wait_45_min();
    return 0;
}