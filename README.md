# Framebuffer Geometry (C version)

A colored clock just in test of SDL library.
I confess this test/exercice idea comes from "Le Vortex" [https://www.youtube.com/@LeVortexARTE/videos](https://www.youtube.com/@LeVortexARTE/videos) and their clock.

## The best way to test :
- Create a Linux virtual machine (VM) with an X server.
- In console mode:
  - Check and configure network between VM and your physical machine (PM).
  - install, configure and run the ssh server.
  - Log off or not, but the VM still active.
- Open a terminal on your PM.
  - Open a ssh connection with the VM.
  - Install and configure the C development kit (gcc + make + lib in debug/dev).
  - Install libsdlX (x = lastest version - here i used SDL2), and its dev-version.
  - Install git.
  - Clone this repository.
  - Go to the newly created directory.
  - Just type ```make```.
  - If all is ok, the VM console shows you nice arts, while the ssh terminal shows some information.

Of course, you can try directly in your PM (in TTY1 for example - CTRL-ALT-F1 if you use X interface). 
**BUT if something wrong, prepare to reboot your PM**.

## Great thanks to
- **HSL and HSV color** :
  - [https://en.wikipedia.org/wiki/HSL_and_HSV](https://en.wikipedia.org/wiki/HSL_and_HSV)
  - [https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/](https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/)

- **SDL** :
  - [https://fr.wikibooks.org/wiki/Cat%C3%A9gorie:Programmation_avec_la_SDL_(livre)](https://fr.wikibooks.org/wiki/Cat%C3%A9gorie:Programmation_avec_la_SDL_(livre))
  - [https://wiki.libsdl.org/SDL2/FrontPage](https://wiki.libsdl.org/SDL2/FrontPage)
  - All people writing about SDL programmation.

- **Circle algorithmes** :
  - [https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d'Andres](https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_cercle_d'Andres)
  - [https://gist.github.com/bert/1085538](https://gist.github.com/bert/1085538)


# ENJOY
