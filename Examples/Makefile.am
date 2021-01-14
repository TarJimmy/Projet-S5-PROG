CC=$(GCC_CROSS)
CCAS=$(GCC_CROSS)
AM_CFLAGS=-mbig-endian
AM_CCASFLAGS=-mbig-endian
AM_LDFLAGS=-nostdlib -T $(srcdir)/linker_script -n --entry main
AM_LDFLAGS+=-Wl,-EB

if HAVE_ARM_COMPILER
noinst_PROGRAMS=example1 example2 example3 example4 testLDR_STR testLDRB_STRB testLDRH_STRH testLDR_SH_SB testLDM_STM test_Data_proc test_Soutenance insertion_sort

all-am: $(PROGS)
endif

example1_SOURCES=example1.s
example2_SOURCES=example2.s
example3_SOURCES=example3.s
example4_SOURCES=example4.s

testLDR_STR_SOURCES=testLDR_STR.s
testLDRB_STRB_SOURCES=testLDRB_STRB.s
testLDRH_STRH_SOURCES=testLDRH_STRH.s
testLDR_SH_SB_SOURCES=testLDR_SH_SB.s

testLDM_STM_SOURCES=testLDM_STM.s

test_Data_proc_SOURCES=test_Data_proc.s

test_Soutenance_SOURCES=test_Soutenance.s

insertion_sort_SOURCES=insertion_sort.c

EXTRA_DIST=linker_script

clean-local:
	$(RM) $(PROGS)
