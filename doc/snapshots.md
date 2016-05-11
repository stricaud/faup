## Faup Snapshots

A Snapshot is splitted fields, with time and counters which correspond to what you could have seen at a given time. This allow you to share this with other people, as well as use it as a database to get information from URLs you have seen in the past. Since faup release 1.5, we have included a capability to read/write/compare snapshots.

You can record something like one day, week, month of seen URLs, and later, you can record a one hour session. You can then use faup to check all records that belong to your session that was not part of your snapshot. The hypothesis is simply based on the fact that users do not usually go to new websites. However malware do. And because this is always in motion, it is great to automate this snapshot creation so you do not have to worry and let faup to the work for you.

### Creating a snapshot

Suppose you have the following file, named first.txt

```
http://www.slashdot.org
https://www.google.com
```

You can create a snapshot like this:

    $ faup -q -s mysnapshot first.txt
    $    

It will create the snapshot, in a file named "mysnapshot.urls" (urls being the extension for url snapshot)

You can now print that snapshot using faup using the builtin shell, like this:

```
    $ faup $ snapshot read mysnapshot.urls 
    {
        "snapshot name": "mysnapshot",
        "snapshot length": 6,
        "items":[               {
                "key": "host",
                "length": 2,
                "values": [
                        {"value": "www.slashdot.org", "count": 1, "first seen": "2016-05-10 23:24:52 -0700", "last seen": "2016-05-10 
23:24:52 -0700"},                                                                                                                    
                        {"value": "www.google.com", "count": 1, "first seen": "2016-05-10 23:24:52 -0700", "last seen": "2016-05-10 23
:24:52 -0700"}                                                                                                                       
                ]
        },
                {
                "key": "domain",
                "length": 2,
                "values": [
                        {"value": "google.com", "count": 1, "first seen": "2016-05-10 23:24:52 -0700", "last seen": "2016-05-10 23:24:
52 -0700"},                                                                                                                          
                        {"value": "slashdot.org", "count": 1, "first seen": "2016-05-10 23:24:52 -0700", "last seen": "2016-05-10 23:2
4:52 -0700"}                                                                                                                         
                ]
        },
	...
```

### Comparing a snapshot with another

Suppose you have another file, named second.txt
```
http://www.slashdot.org
https://www.google.co.uk
```

In this case, only the tld "co.uk" has not been seen, but the domain has.

We create the snapshot like this:

   $ faup -q -s secondsnap second.txt 


To check the different between second and first and get only what exists in second which is not in first, run the following command:

```
   $ faup -s mysnapshot -c secondsnap
   {
        "snapshot name": "secondsnap-mysnapshot",
        "snapshot length": 3,
        "items":[               {
                "key": "domain",
                "length": 1,
                "values": [
                        {"value": "google.co.uk", "count": 1, "first seen": "2016-05-10 23:31:34 -0700", "last seen": "2016-05-10 23:3
1:34 -0700"}                                                                                                                         
                ]
        },
                {
                "key": "tld",
                "length": 1,
                "values": [
                        {"value": "co.uk", "count": 1, "first seen": "2016-05-10 23:31:34 -0700", "last seen": "2016-05-10 23:31:34 -0
700"}                                                                                                                                
                ]
        },
                {
                "key": "host",
                "length": 1,
                "values": [
                        {"value": "www.google.co.uk", "count": 1, "first seen": "2016-05-10 23:31:34 -0700", "last seen": "2016-05-10 
23:31:34 -0700"}                                                                                                                     
                ]
        }
        ]

	}
```

Which displays the json of the differences.
