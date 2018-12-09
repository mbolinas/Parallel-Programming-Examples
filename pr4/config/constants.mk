# Makefile constants

# If true, it will use ./config/constants_local.mk 
# when the environment is not on EECIS Unix System
USER_DEF := false

# DO NOT MODIFY BELOW

OS := $(shell uname)
HOST := $(shell hostname)
ROOT := ./config

# General Constants

CC = cc -pedantic -std=c11
MPICC = mpicc -pedantic -std=c11
PTCC = $(CC) -pthread
OMPCC = $(CC) -fopenmp
NVCC = nvcc --compiler-bindir mpicc

LIB_MATH = -lm
LIB_GD = -lgd

FFMPEG = ffmpeg

MOVIE_FLAGS = -movflags faststart -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2"

# Environment Determined Constants

ifeq ($(USER_DEF), true)
	include $(ROOT)/constants_local.mk
else ifeq ($(OS), Darwin)
	include $(ROOT)/constants_osx.mk
else ifeq ($(HOST), cisc372)
	include $(ROOT)/constants_cisc372.mk
else
	include $(ROOT)/constants_linux.mk
endif