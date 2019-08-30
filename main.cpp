#include <Windows.h>
#include <stdio.h>
#include <iostream>

#include "multicompute.h"

template<uint32_t WIDTH>
class ImageLine
{
public:
    ImageLine() : tid(-1), line(0), data(nullptr) {}
    ImageLine(uint32_t line, uint8_t* data) : tid(-1), line(line), data(data) {}
    ImageLine(const ImageLine& l) : tid(-1), line(l.line), data(l.data) {}
    ~ImageLine() {}

    inline bool empty() const { return (nullptr == data); }
    inline int getThreadId() const { return tid; }
    inline void setThreadId(int id) { tid = id; }
	inline uint32_t getLineNum() const { return line; }
	inline static void doSomething() { x = (uint32_t)(rand() * 100); printf("Processing image line %d, value = %g\n", line, x); }
    inline const uint8_t* getData() const { return data; }

private:
    int tid;
    uint32_t line;
    uint8_t* data;
	uint32_t x;
};

int main(int argc, char** argv)
{
    // the image is 1855 * 1307
    std::vector<uint8_t> image(1855*1307, 0);
    ResourcePool<ImageLine<1307>> resources;

    // Init resource
    for (uint32_t i = 0; i < 1855; i++)
        resources.push(ImageLine<1307>(i, image.data() + (i * 1307)));

    // 
  //  multiCompute<ImageLine<1307>>(resources, [](const ImageLine<1307>& e) {
  //      //printf("[Thread #%d] Processing image line %d\n", e.getThreadId(), (int)e.getLineNum());
		//e.doSomething();
  //  });
	multiCompute<ImageLine<1307>>(resources, ImageLine<1307>::doSomething());

    system("pause");
}