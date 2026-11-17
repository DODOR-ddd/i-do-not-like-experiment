CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# src/ 下每个 .cpp 对应一节实验，各自编译成 bin/ 下同名可执行文件
BINS := $(patsubst src/%.cpp,bin/%,$(wildcard src/*.cpp))

all: $(BINS)

bin/%: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -MMD -MP -o $@

# make run-1.1：需要时重新编译，然后运行 1.1 这一节
run-%: bin/%
	@./$<

# make watch NAME=1.1：src 下任何文件变动就自动重编译并运行
watch:
	@find src -type f | entr -c make run-$(NAME)

clean:
	@rm -rf bin

.PHONY: all watch clean

-include $(BINS:bin/%=bin/%.d)
