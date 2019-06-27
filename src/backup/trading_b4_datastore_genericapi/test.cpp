#include <rapidjson/document.h>
#include <cstdio>

int main(int, char*[])
{
    const char json[]= " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    printf("Original JSON:\n %s \n", json);

    rapidjson::Document document;

#if 0
    if (document.Parse(json).HasParseError())
        return 1;
#else
{
    char buffer[sizeof(json)];
    memcpy(buffer, json, sizeof(json));
    if (document.ParseInsitu(buffer).HasParseError())
        return 1;
}
#endif

    printf("\nParsing to document succeeded.\n");

}