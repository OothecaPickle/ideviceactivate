CFLAGS := -g -O0 `pkg-config --cflags libimobiledevice-1.0 libplist libcurl`
LDFLAGS := `pkg-config --libs libimobiledevice-1.0 libplist libcurl`

all:
	$(CC) -o ideviceactivate ideviceactivate.c activate.c cache.c util.c $(CFLAGS) $(LDFLAGS)
	install_name_tool -change libplist.1.dylib /opt/local/lib/libplist.1.dylib ./ideviceactivate
