#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

xmlXPathObjectPtr
getnodeset(htmlDocPtr doc, xmlChar *xpath)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(doc);

    if (context == NULL)
    {
        printf("Error in xmlXPathNewContext\n");
        return NULL;
    }

    result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);

    if (result == NULL)
    {
        printf("Error in xmlXPathEvalExpression\n");
        return NULL;
    }

    if (xmlXPathNodeSetIsEmpty(result->nodesetval))
    {
        xmlXPathFreeObject(result);
        printf("No result\n");
        return NULL;
    }

    return result;
}

int main(int argc, char* args[])
{
    htmlDocPtr doc;
    xmlChar *xpath = (xmlChar*) "//big";
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;
    xmlChar *keyword;
    CURL *curl;
    CURLcode res;
    FILE *fp;
    char output[255];

    char *url = "https://en.wikipedia.org/wiki/List_of_Presidents_of_the_United_States";

    curl = curl_easy_init();
    if(!curl) return 1;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    strcpy(output, basename(url));
    strcat(output, ".html");

    fp = fopen(output, "wb");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Could not download file: %s\n",
        curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);    

    doc = htmlParseFile(output, "utf-8");

    if (doc == NULL)
    {
        fprintf(stderr, "Document not parsed successfully. \n");
        return 1;
    }

    result = getnodeset (doc, xpath);

    if (result)
    {
        nodeset = result->nodesetval;
        for ( int i = 0; i < nodeset->nodeNr; i++ )
        {
            keyword = xmlNodeGetContent(nodeset->nodeTab[i]);
            printf("Keyword: %s\n", keyword);
            xmlFree(keyword);
        }
        xmlXPathFreeObject(result);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
