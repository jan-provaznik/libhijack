.PHONY:
all: \
	build/libhijackfopen.so \
	build/libhijackioctl.so \
	build/testfopen \
	build/testfopen.wrap \
	build/testioctl \
	build/testioctl.wrap

.PHONY:
clean:
	rm -rf build

build:
	mkdir -p build

build/lib%.so: %.c | build
	cc -fPIC -shared -Wl,--no-as-needed -ldl $< -o $@

build/test%.wrap: test/test%.wrap | build
	cp -a $< $@

build/test%: test/test%.c | build
	cc $< -o $@

