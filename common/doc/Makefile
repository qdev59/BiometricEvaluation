#
# This software was developed at the National Institute of Standards and
# Technology (NIST) by employees of the Federal Government in the course
# of their official duties. Pursuant to title 17 Section 105 of the
# United States Code, this software is not subject to copyright protection
# and is in the public domain. NIST assumes no responsibility whatsoever for
# its use by other parties, and makes no guarantees, expressed or implied,
# about its quality, reliability, or any other characteristic.
#
ROOTNAME = BECommonFramework
SOURCETEX = $(ROOTNAME).tex title.tex intro.tex overview.tex framework.tex error.tex io.tex time.tex image.tex video.tex process.tex system.tex memory.tex text.tex feature.tex finger.tex view.tex datainterchange.tex parallel.tex messaging.tex frameworkbuild.tex mpijob.tex device.tex
SOURCECODE = ../src/include/*.h
export BIOMEVAL_VERSION = $(shell eval `cat ../VERSION | egrep ^[A-Z]`; echo $$MAJOR_VERSION.$$MINOR_VERSION)
LOCALINC=../../doccommon

COMMONTEX=$(LOCALINC)/texfiles/commonglossary.tex $(LOCALINC)/texfiles/commoncommands.tex
COMMONASSETS=../../doccommon/assets/nistident_cent_vec.pdf ParallelJob.pdf
include $(LOCALINC)/common.mk

.DEFAULT_GOAL = all

all:
	$(MAKE) WRAPPER=BEFull GENERATEDOXYGEN=YES GENERATEINDEX=YES INDEXFILE=BEFull thedoc
small:
	$(MAKE) WRAPPER=BESmall GENERATEDOXYGEN=NO GENERATEINDEX=YES INDEXFILE=BESmall thedoc
