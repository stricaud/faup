## Faup Library API

The Faup library is designed to be embeded with no pain. The library is lightweight and the license is very permissive, so you can do whatever you want to. We just want to fix the url normalization problem.

A great way to see how the C library can be used is to look at the [faup][fauptoolsrc] command line tool source code.

### Python API

```python
from pyfaup.faup import Faup
import sys
import codecs

fp = Faup()
file_urls=codecs.open(sys.argv[1],'r','ascii',errors='ignore')
urls=file_urls.readlines()

for url in urls:
    url=url.replace('\n','')
    print("URL:[%s]" % (url))
    f.decode(url)
    print("-----> Extracted TLD:%s" % f.get_tld())

```


### C API



[fauptoolsrc]: http://github.com/stricaud/faup/blob/master/src/tools/faup.c

