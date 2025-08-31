CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic -I/code/fltk -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -pthread -DUSE_SYSTEM_LIBDECOR=0 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT 
LDFLAGS = -lsodium -lyaml-cpp -pthread -rdynamic /code/fltk/lib/libfltk.a -pthread -lpthread -lm -lwayland-cursor -lwayland-client -lxkbcommon -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lcairo -lX11 -ldl -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lcairo -lXinerama -lXcursor -lXfixes -lXrender -lX11
TARGET = fltk_messenger

SRCS = main.cpp configuration.cpp port_dialog.cpp messenger_window.cpp state_machine.cpp thread_safe_fifo.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

