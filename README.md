# Simple Feed #

This module creates JSON feed endpoints that output node and taxonomy term resource representations. You can have multiple profiles, each with its own security and output settings, which allows you to finetune the representation for each profile based on what's needed. The feed is primarily designed for other backend infrastructure to quickly fetch data (1 way) and does not allow incoming requests to make changes to the database.

## Sounds Similar to [Services](https://www.drupal.org/project/services)? ##

This module was actually inspired by it; but because this is just a simple JSON feed, it's designed to be quicker to set up. Simply add a profile in the config page, go through one page of settings, enable the feed, and it's up and running.

Apart from that, there are a few specific advantages:

1. **Multiple entity queries**: if we know the specific node ids needed, a single HTTP request will get the job done.

1. **URL aliases**: added options to include all URL aliases of an entity in the representation. Helpful for sites setup that allows multiple URL aliases per page.

1. **Easy to access**: because it does not require login, it is easy for other back-end infrastructures to set up their access, but we do provide security measures to set access-key, restrict HTTP referers, and restrict IP addresses.

## How to set up ##

* Add the extracted content from .zip to modules folder.

* Enable Module in the admin/modules page.

* Go to Module's configuration page.

* Click **+ Add Profile** (A default_profile is also generated as an example).

* Enter Profile name and Description, choose whether to enable immediately, and press **Save**.

* Click **Edit** under **Operations** column in the Profile table for the one you just created.

* Go through the settings and **Save**.

* Click the checkbox next to your profile and **Save**.

* Your new feed will be available at ```this-is-my-site/simple_feed/my-profile``` (Note: Normally, the access-key is meant to be appended in the HTTP header; to access in browser, you can enable a setting that allows appending access-key in the "key" query argument).

* If you want to keep your profile, but disable it, you can do it by unchecking the checkbox for your profile in the table.

## How to use ##

* For each profile, there are 5 endpoints available: ```/summary```, ```/node```, ```/nodes```, ```/taxonomy_term```, ```/taxonomy_terms```. For example, you would go to ```this-is-my-site/simple_feed/my-profile/summary``` to access the /summary endpoint.

* **Summary:** based on the **node types** and **taxonomy term types** enabled in *Entity availability settings*, it will output all the node and taxonomy term representations with attributes you set in the *Summary settings*. This is useful to get a quick overview on all the entities you have access to.

* **Node:** outputs a single node representation with attributes set in *Entity query settings*. Will respect settings in *Entity availability settings* (e.g., only chosen node types will be returned).

* **Nodes:** Similar to Node but outputs multiple node representations. You could query multiple nodes like ```/nodes/1,2,3,4,5``` and all 5 will be returned if they exist and you are allowed access to them. Nodes that aren't found will simply be skipped.

* **Taxonomy Term:** outputs a single term representation with attributes set in *Entity query settings*. Will respect settings in *Entity availability settings*

* **Taxonomy Terms:** shares constraints with Taxonomy Term and will output multiple term representations similar to Nodes.

## Future Plans ##

Some ideas for improvements:

* Create a DRUSH interface to quickly set up profiles
* Import/Export Settings
* Allow query string to take in specific attributes to fetch (instead of a fixed format)
* Although not what was intended, a pagination mechanism could be implemented for ```/nodes```, ```/taxonomy_terms```. This may reduce chances of server timeouts if too many things are being requested at once
* More security mechanisms easy for non-browsers to interact with.

## How to contact me ##

I can be contacted at hopokuan@umich.edu
