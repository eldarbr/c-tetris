CC = gcc
CCFL = -Wall -Werror -Wextra --std=c11
BUILD_LIBS = -lcheck

INSTALLATION_DIR := ../bin

LIB_SRC_DIR := game/tetris
LIB_OBJ_DIR := .obj_lib
LIB_SRC_FILES := $(wildcard $(LIB_SRC_DIR)/*.c)
LIB_OBJ_FILES := $(patsubst $(LIB_SRC_DIR)/%.c,$(LIB_OBJ_DIR)/%.o,$(LIB_SRC_FILES))
TESTS_SRC_DIR := $(LIB_SRC_DIR)/tests
TESTS_OBJ_DIR := .obj_tests
TESTS_SRC_FILES := $(wildcard $(TESTS_SRC_DIR)/*.c)
TESTS_OBJ_FILES := $(patsubst $(TESTS_SRC_DIR)/%.c,$(TESTS_OBJ_DIR)/%.o,$(TESTS_SRC_FILES))
COMMON_SRC_FILES := common/*.c
GAME_SRC_FILES := tetris.c gui/cli/*.c $(COMMON_SRC_FILES)
DIST_PACKAGE = tetris-1.0.tar.gz

OS := $(shell uname -s)
ifeq ($(OS), Darwin)
	CCFL += -DOS_MAC
endif
ifeq ($(OS), Linux)
	CCFL += -DOS_LINUX
	BUILD_LIBS += -lpthread -lm -lsubunit
endif

all: game

test: $(TESTS_OBJ_FILES) $(LIB_OBJ_FILES) $(COMMON_SRC_FILES)
	$(CC) $(CCFL) -o test.out $^ $(BUILD_LIBS)
	./test.out

gcov_report: clean
	$(CC) $(CCFL) -fprofile-arcs -ftest-coverage $(TESTS_SRC_FILES) $(LIB_SRC_FILES) $(COMMON_SRC_FILES) -o test_report.out -lm $(BUILD_LIBS)
	./test_report.out
	lcov -t test_report -o test.info -c -d .
	genhtml -o report test.info
	open ./report/index.html

dist:
	tar -czvf $(DIST_PACKAGE) --ignore-failed-read \
		game gui common tetris.c Doxyfile Makefile

tetris_lib.a: $(LIB_OBJ_FILES)
	ar rcs $@ $^

game: tetris_lib.a
	$(CC) $(CCFL) $(GAME_SRC_FILES) tetris_lib.a -lm -lncurses -o tetris

install: prepare_inst game
	mv tetris $(INSTALLATION_DIR)

uninstall:
	rm -rf $(INSTALLATION_DIR)

dvi:
	rm -rf docs
	@doxygen
	open docs/html/index.html

$(LIB_OBJ_DIR)/%.o: $(LIB_SRC_DIR)/%.c prepare_l
	$(CC) $(CCFL) -c -o $@ $<

$(TESTS_OBJ_DIR)/%.o: $(TESTS_SRC_DIR)/%.c prepare_t
	$(CC) $(CCFL) -c -o $@ $<

prepare_l:
	mkdir -p $(LIB_OBJ_DIR)

prepare_t:
	mkdir -p $(TESTS_OBJ_DIR)

prepare_inst:
	mkdir -p $(INSTALLATION_DIR)

clean:
	rm -rf .obj* tetris_lib.a tetris test.out *.o
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.info
	rm -f test_report.out
	rm -rf report docs
	rm -rf $(DIST_PACKAGE)

rebuild: clean all

pre-commit:
	find . \( -name '*.c' \) -print0 | xargs -t -0 -n 1 $(CC) $(CCFL) -c
	make clean

format:
	find .. \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 clang-format -n -Werror --verbose --style=google

format-force:
	find .. \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 clang-format -i --verbose --style=google
