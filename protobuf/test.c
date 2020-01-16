#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "4G.pb-c.h"


int main(void)
{
    LocationInfo locInfo = LOCATION_INFO__INIT;
    location_info__init(&locInfo);
    locInfo.timestamp = 1;
    locInfo.latitude = 2;
    locInfo.longitude = 3;
    locInfo.altitude = 4;
    size_t len = location_info__get_packed_size(&locInfo);
    printf("len = %ld\n", len);

    uint8_t *buf = (uint8_t*)malloc(len + 1);
    
    location_info__pack(&locInfo, buf);

    if (buf != NULL) {
        int i = 0;
        for (i = 0; i < len; i++) {
            printf("%X ", buf[i]);
        }
        printf("\n\n");
    }

    ProtobufCAllocator* allocator = NULL;
    LocationInfo * locRes = location_info__unpack(allocator, len, buf);

    if (locRes != NULL) {
        printf("locInfo.timestamp = %ld\n", locInfo.timestamp);
        printf("locInfo.latitude = %f\n", locInfo.latitude);
        printf("locInfo.longitude = %f\n", locInfo.longitude);
        printf("locInfo.altitude = %f\n", locInfo.altitude);
    }
    location_info__free_unpacked(allocator, locRes);

    return 0;
}
