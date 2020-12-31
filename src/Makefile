CFLAGS := -g -O0 `pkg-config --cflags libimobiledevice-1.0 libplist-2.0 libcurl`
LDFLAGS := `pkg-config --libs libimobiledevice-1.0 libplist-2.0 libcurl`

all:
	$(CC) -o ideviceactivate ideviceactivate.c activate.c cache.c util.c $(CFLAGS) $(LDFLAGS)
