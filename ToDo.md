# ToDo

## Important

* [x] Fix up the super sentinel lasers:
    * [x] Proper sprites.
    * [x] Proper hitboxes.
* [x] Boss fight woo.
    * [x] The same one worked on in versions 15 ad 16.
    * [x] BUT BETTER.
    * [x] Strafing firing bullets.
    * [x] Bouncing from side to side with LASERS.
    * [x] Pincer attack.
    * [x] Okay, so it is very similar, but this time it will work.
    * [x] Boss entry.
    * [x] Boss exit:
        * [x] More explosions YAY.
        * [x] Spinny roundy parts
    * [x] Fix the lasers because I broke them:
        * [x] PAIN.
        * [x] Laser sprites are wrong, idk why.
    * [x] Exclamation marks BANG just before firing the lasers or...
    * [x] Pilot lasers for the lasers.
    * [x] Fix up the boss fight text.
    * [x] The lazers sometimes remain for too long when transitioning to the last phase.
        * [x] They may not be despawned correctly or not at all.
* [x] Fix ghost bullets when going from 8 enemies to the boss.
* [x] Add an option to the main menu for improvements/next version.
* [x] Scanning sector for extra information.
    * [x] Sectors
    * [x] Boss
* [x] Basically spruce up the pause menu.
* [x] Last stage is a bit too difficult when compared to the boss, maybe leave it as a challenge?
* [x] Potentially add a way to see the bosses health directly, maybe in the scan function?
* [x] Title screen/background.
* [x] Different boss background.

## Not important things and ideas

* [ ] Portal out of the sector / Level transition.
* [ ] Loading screen - before the game starts.
    * [ ] Kinda similar to an old computer booting.
* [ ] Ahh yes, a tutorial would be cool.
* [ ] Custom background for tutorial.
    * Maybe with a station or big ship in the background, something like that.
    * Maybe like you are with a fleet.
* [ ] Scroll ui interface.
    * Mainly for the credits if they get too large.
        * I don't know if they will though.
* [ ] Scoreboard.
    * [ ] Different screens for easy, normal, hard.
* [ ] Stats.
    * [ ] Keep track of things like time played and deaths.
    * Half implemented already because the check collision functions return the type of the collision.
* [ ] Custom seed input.
* [ ] Figure out how to layer backgrounds.
    * [ ] Make the screen boarder the top most layer.
        * So other stuff is drawn underneath it.
    * [ ] Also when the enemies portal in they are drawn behind the background.
        * Kinda not visible when not looking for it, but it annoys me.
        * SO FIX IT FUTURE ME.
    * We have some more freedom then.
    * May be something to do with it being BgType_Bmp8.
    * Try other stuff and see if it works.
* [ ] If there becomes too much sprite GFX to be loaded all at once then use smart loading where only the necessary
  sprites for the current activity are loaded.
    * It is already set up for loading the sprites, just the unloading needs to be done. ONLY if necessary.
* [ ] Dad suggested the idea of the player being able to take multiple hits.
    * Could be an interesting idea to play around with.
* [ ] Slap a bunch of the scanning function code into smaller helper functions to make the functions easier to write and
  read.
* [ ] Two ideas for new interface types:
    * [ ] Text interface, just a wall of text, auto-fits the text to the given area.
    * [ ] Scroll interface, when there are too many options to fit on the screen at once you can scroll?
        * Maybe just use sub interfaces, but ehh.
        * Potentially the text interface could scroll instead.
* [ ] Maybe come up with a way to make the normal enemy text screen a little more interesting.
* [ ] Continue to challenge from beating the boss the first time.