# π structure
+ Version control built into the language
+ Trees with full linearity and interval selection
  + Must be able to continuously move, esp along grid coordinates: that is, a single Cartesian direction should rarely have a stop point
  + Change directions sparingly: momentum is valuable


## Mechanics
+ Trivial `eval` function
  + Evaluation is sequential with stack-like context
  + Register-style variables
  + `eval` instead of functions, like Joy
+ Declarative ξ topology
+ APL-style declarative operator mods: `/+` to sum, for example
  + Prefix-tree parsed: `/` accepts `<binop>` as a successor, give or take
  + Polymorphic interpretation: `/+1 2 3` sums a vector, `/+α` sums elements in the _α_ stream
  + Static typing with easy coercion
+ Generational, stop-the-world, mark/sweep GC

**Q:** is UTF9 fast enough to use as the bytecode format, or should we have a dedicated π bytecode? UTF9 is nice and homoiconic, so I'd like to use it if we can. Maybe trace → JIT the decoding process if it's too slow.


## Program encoding
π supports both toplevel connection requests ([Ξ](Xi.md)) and local imperative scripting. Ξ is treated as an imperative command that modifies the [Φ](Phi.md)/[Γ](Gamma.md) topology.


### Ξ example
Let's take the program I wrote in [π-ni](pi-ni.md):

```
*.xml ^[k↓2↑!/_files.xml/ /"([^"]+(?:\.tar|\.zip))"/<] \
      ¨/+["https://archive.org/download" a '/ b] \
      ë[wget -c]
```

This is not real code, but it's a rough translation of how π would see the logic. It might contain the set of keystrokes you would use to create it. At the top there are four pieces, each a ϝ component:

+ `*.xml`
+ `^[]`
+ `¨/+[]`
+ `ë[]`

We'd encode this as a Ξ, which internally uses [γ₀](gamma0.md):

```
(Ξ,
  ((wildcard, "*.xml"),  ← "wildcard" is a utf9 symbol
   (^, (...)),           ← ^ is also a symbol
   (¨/+, (...)),
   (ë, ("wget", "-c")),
   (fd_out, 1)),         ← implied, probably?

  ((0, 1, 1, 0),         ← ξ connection list
   (1, 1, 2, 0),           (γ₁, φ₁, γ₂, φ₂)
   (2, 1, 3, 0),
   (3, 1, 4, 0)))
```

We probably leave it up to the runtime to connect any not-yet-connected ε streams, e.g. for debugging. ϝs should adhere to a standard debug/metadata protocol, kind of like logging, so we get reasonable status updates in whichever UI we're using to drive the pipeline.

OK, so what's in the `(...)` tuples? A π expression in each case. `[...]` becomes `(tuple, ...)`; `^` and `¨/+` each evaluate their arguments, but `[]` produces a quoted tuple.

See [π types](pi-types.md) for the mechanics of how each function is parsed and encoded.


### ETL example
I've downloaded a bunch of tweets from the [ArchiveTeam dataset](https://archive.org/details/twitterstream). I've extracted them to `.bz2` files, each of which is a stream of `jsonl` objects that looks like this:

```
{"created_at":"Mon Jul 01 02:29:00 +0000 2019","id":1145519644616224768,"id_str":"1145519644616224768","text":"\uacc4\ub2e8\uc9c0\uae30\ub3c4 \uc0ac\ub78c\uc778 \ub9cc\ud07c \ubb34\ub8cc\ud568\uc5d0\ub294 \uc57d\ud55c \uba74\ubaa8\ub97c \ubcf4\uc774\uc9c0\uc694. \uacc4\ub2e8\uc744 \ucc3e\uc544\uc624\ub294 \uc190\ub2d8\uc774 \uc5c6\uc744 \ub54c\ub294 \uacc4\ub2e8\uc9c0\uae30\ub294 \uadf8\uc800 \uacc4\ub2e8\uc744 \ub458\ub7ec\ubcf4\uba70 \ubc29\ubb38\uc790\uac00 \ub098\ud0c0\ub098\uae30\ub97c \uae30\ub2e4\ub9b4 \uc218\ubc16\uc5d0 \uc5c6\ub2f5\ub2c8\ub2e4. (\uc2f1\uae0b)","source":"\u003ca href=\"http:\/\/twittbot.net\/\" rel=\"nofollow\"\u003etwittbot.net\u003c\/a\u003e","truncated":false,"in_reply_to_status_id":null,"in_reply_to_status_id_str":null,"in_reply_to_user_id":null,"in_reply_to_user_id_str":null,"in_reply_to_screen_name":null,"user":{"id":1331213827,"id_str":"1331213827","name":"\uacc4\ub2e8\uc9c0\uae30","screen_name":"Stairkeeper_bot","location":"\uc774 \uc138\uacc4 \uc5b4\ub518\uac00 \uc138\uc6cc\uc9c4 \ub098\uc120\uacc4\ub2e8","url":null,"description":"\ub098\uc120\uacc4\ub2e8\uc744 \uc9c0\ud0a4\ub294 \uacc4\ub2e8\uc9c0\uae30\uc785\ub2c8\ub2e4. 30\ubd84\ub9c8\ub2e4 \uc790\ub3d9\ud2b8\uc717\ud558\uba70 \ud2b9\uc815 \uc2dc\uac04\ub9c8\ub2e4 \ud2b8\uc717\ud569\ub2c8\ub2e4. \uc5ec\ub7ec\ubd84\uaed8 \ub3c4\uc6c0\uc774 \ub418\ub294 \ub9d0\uc744 \ud574\ub4dc\ub9ac\uace0 \uc2f6\uc740, \ubbf8\uc219\ud55c \uacc4\ub2e8\uc9c0\uae30\uc785\ub2c8\ub2e4. \ucc28\ucc28 \uc5ec\ub7ec\ubd84\uc744 \uc54c\uc544\uac00\uaca0\uc2b5\ub2c8\ub2e4. \ubd07\uc8fc:@ValkyrieMH","translator_type":"none","protected":false,"verified":false,"followers_count":13,"friends_count":23,"listed_count":0,"favourites_count":4,"statuses_count":78203,"created_at":"Sat Apr 06 09:57:42 +0000 2013","utc_offset":null,"time_zone":null,"geo_enabled":false,"lang":null,"contributors_enabled":false,"is_translator":false,"profile_background_color":"C0DEED","profile_background_image_url":"http:\/\/abs.twimg.com\/images\/themes\/theme1\/bg.png","profile_background_image_url_https":"https:\/\/abs.twimg.com\/images\/themes\/theme1\/bg.png","profile_background_tile":false,"profile_link_color":"1DA1F2","profile_sidebar_border_color":"C0DEED","profile_sidebar_fill_color":"DDEEF6","profile_text_color":"333333","profile_use_background_image":true,"profile_image_url":"http:\/\/abs.twimg.com\/sticky\/default_profile_images\/default_profile_normal.png","profile_image_url_https":"https:\/\/abs.twimg.com\/sticky\/default_profile_images\/default_profile_normal.png","profile_banner_url":"https:\/\/pbs.twimg.com\/profile_banners\/1331213827\/1365947495","default_profile":true,"default_profile_image":false,"following":null,"follow_request_sent":null,"notifications":null},"geo":null,"coordinates":null,"place":null,"contributors":null,"is_quote_status":false,"quote_count":0,"reply_count":0,"retweet_count":0,"favorite_count":0,"entities":{"hashtags":[],"urls":[],"user_mentions":[],"symbols":[]},"favorited":false,"retweeted":false,"filter_level":"low","lang":"ko","timestamp_ms":"1561948140659"}^M
```

This tweet isn't that interesting because we don't have any hashtags, but I could imagine faceting by author or by time, then sorting in some way, then piping the result through `zstd` to compress into bundles.

**TODO:** explore this at some point
