#include <spargel/base/string/string.h>
#include <stdio.h>
#include <assert.h>

int main()
{
    spargel::base::string str1;
    assert(str1.length() == 0);
    spargel::base::string str2("hello");
    assert(str2.length() == 5);
    spargel::base::swap(str1, str2);
    assert(str1.length() == 5);
    assert(str2.length() == 0);
    spargel::base::string str3(str1);
    assert(str3.length() == 5);
    assert(str3[4] == 'o');
    spargel::base::string str4(spargel::base::move(str1));
    assert(str4.length() == 5);
    assert(str1.length() == 0);

    printf("%s\n", str4.c_str().data);
    return 0;
}
