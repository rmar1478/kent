kentSrc = ../../../..
GBROOT = ..
include ${GBROOT}/common.mk

SUBDIRS = lib download process align copy dbload \
	selectWithPsl gbSanity gbGetSeqs gbConf repair ccds extFileUpdate refSeqCheck\
	lsSnpPdb

all: ${SUBDIRS:%=%.suball}
%.suball:
	${MAKE} -C $* all

clean: ${SUBDIRS:%=%.subclean}
	rm -rf ${GB_BINDIR}

%.subclean:
	${MAKE} -C $* clean

