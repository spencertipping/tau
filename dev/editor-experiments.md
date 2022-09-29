# Editor experiments
I'm working on [π₂](../doc/pi2.md) and looking for inspiration, so let's download a bunch of technical editors and see how usable they are. [Here's one such list.](https://tex.stackexchange.com/questions/57068/wysiwyg-latex-editor-for-maths)

+ EqualX
+ [MathCha](https://mathcha.io/editor)
+ LyX
+ [TeXmacs](https://askubuntu.com/questions/694304/texmacs-not-available-ubuntu-15-10)

```sh
$ echo 'deb http://download.opensuse.org/repositories/home:/slowphil:/texmacs-devel/xUbuntu_22.04/ /' | sudo tee /etc/apt/sources.list.d/home:slowphil:texmacs-devel.list
$ curl -fsSL https://download.opensuse.org/repositories/home:slowphil:texmacs-devel/xUbuntu_22.04/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/home_slowphil_texmacs-devel.gpg > /dev/null
$ sudo apt update
$ sudo apt install texmacs
```

```sh
$ sudo apt install lyx equalx
```


## TeXmacs observations
+ Fonts look ok, but editor is clunky
+ If selection crosses a container boundary, the whole container becomes selected (no partial boxes if anything is outside)


## LyX observations
Very basic-feeling; ergonomics are terrible. I have to use the toolbar to insert math, no native syntax integration like `\\` to insert TeX.


## EqualX observations
Just a TeX renderer, like emacs+auctex. Clunky and not very useful IMO.


## Overall
I prefer spacemacs with font-lock to all of these. It feels responsive and has good keybindings, and font-lock with eziam/tao themes makes most markdown syntax unobtrusive. I can lay out my documents the way I want to. Proportional fonts look good in prose, although it's awkward to mix them with code.

I'm not seeing anything very compelling in these editors, probably because I'm looking in the wrong place. Math notation looks great and is somewhat useful for APL-style code, but [π₂](../doc/pi2.md) is about more than just math-as-code.
