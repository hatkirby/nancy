# nancy
nancy is a simple twitter bot inspired by the following tweet:

<blockquote class="twitter-tweet" data-lang="en"><p lang="en" dir="ltr">Nancy Drew and the Secret of the Ovipositor</p>&mdash; ruben ferdinand✨ (@urbanfriendden) <a href="https://twitter.com/urbanfriendden/status/707544674349797378">March 9, 2016</a></blockquote>
<script async src="//platform.twitter.com/widgets.js" charset="utf-8"></script>

It tweets a new Nancy Drew ebook title every three hours. Currently, it uses the [WordNet database](http://wordnet.princeton.edu/wordnet/) to pick adjectives and nouns describing the Nancy's latest misadventures.

The bot uses [YAMLcpp](https://github.com/jbeder/yaml-cpp) to read a configuration file, MySQL C API to read the WordNet database (seeded with [this MySQL version of WordNet 2.0](androidtech.com/html/wordnet-mysql-20.php)), and [twitcurl](https://github.com/swatkat/twitcurl) to send the status updates to Twitter.

The canonical nancy handle is [@nancy_ebooks](https://twitter.com/nancy_ebooks).
