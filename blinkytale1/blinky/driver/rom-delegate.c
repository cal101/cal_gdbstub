

/*
 * A simple stupid way to replace some big library methods 
 * with methods available in firmware rom to save some iram space.
 */


// https://gcc.gnu.org/onlinedocs/gccint/Soft-float-library-routines.html
// https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html

static double (* const rom__muldf3) (double a, double b) = (void*) 0x4000c8f0; 

// spare some iram by using rom symbol. should be done on linker/library level.
double __muldf3 (double a, double b) {
    return rom__muldf3(a, b);
}

static long (* const rom__muldi3) (long a, long b) = (void*) 0x40000650;

long __muldi3 (long a, long b) {
	return rom__muldi3(1, b);
}

//void *memcpy(void *str1, const void *str2, size_t n) __attribute__((alias("ets_memcpy")));
void *ets_memcpy(void *str1, const void *str2, unsigned int n);

//void *memcpy(void *str1, const void *str2, unsigned int n) __attribute__((alias("ets_memcpy")));
void *memcpy(void *str1, const void *str2, unsigned int n) {
	return ets_memcpy(str1, str2, n);
}
