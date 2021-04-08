# portf

Is a cli tool for managing your stonks portfolio. Originally it started as a
rust project but i didn't get anywhere so here is take 2.

Disclaimer: This is highly in development and i am not sure how everything will
turn out, so you shoudn't use it.

## Idea

The Idea is that i will split the application in multiple components:

- A library that handles all logic.
  - Doing api requests
  - Processing api resusts
  - Writing to local db
  - Reading from local db
- All these functions can be invoked by a interface
  - Provide a command line interface so people can do
    - scripting with it
    - integrate it in statusbar
    - write notifications with dunst
  - A more modern tui utilizing neovim
    - Look at portfolio / stonks you follow
    - Add new stonks
    - Create watchers
    - etc ...
  - Maybe more to come, like right now i am thinking
    - python + matplotlib to draw graphs
    - or maybe with c++ & qt
    - or maybe there is a library where we can draw graphes in terminal,
      maybe we can even write something in lua for this and we write the output
      to a neovim buffer

![Idea](docs/idea.png)
*Visualizes the idea of this tool (does not follow any uml standard, its a
sketch)*

Disclaimer: I don't think we have to do a lot of async things because we mainly
operate on the local database and the sync will happen either as cronjob or as
daemon (more about that below). I am more interested in keeping it simple and
supid (kiss).

## Workflow

- You have a local database with a lot of stonks

- Sync:
  - Either (still have to decide about that):
    - Cronjob `portf -Sy` to update local database maybe every x minutes
    - Daemon that you start on startup and updates local database every x
      minutes
    - We need multiple APIs because most have a upper limit on how many request
      you can do per day (yahoo finance low latency 100 request per day).
    - Also we can only get information for 10 stonks at the same time. So we
      have to do mutliple requests. (`curl_multi_add_handle`)
- Search:
  - Interface probably `portf -Ss phrase`
    - Maybe we want something like yay search here, were you can pick
      For example i want 1, 4, 6
      And then you start tracking them
  - Use api request completion to go from `Apple` to `AAPL`
    - Nobody knows the correct symbols
- Tracking/Portfolio:
  - Local db is split up in multiple tables
    - Stonk table which holds all stonks you watch, you don't have to own
      shares for them
    - Being able to have multiple portfolios
      Like portfolio memes holds shares to memes (Gamestop, AMC)
      Like portfolio long holds shares for long term things (AAPL)
      Like portfolio gamble contains all nonesense things, idk
    - Being able to get a overview for one or many portfolios
      +/- for day/week/month/year/since_bought
      etc
  - Watchers which will send notifications when a case happens
    - e.g. AAPL falls below $100
    - or maybe even more difficult things (Maybe there is some news api we can
      watch and crawl)

Disclaimer: command line interface inspired by pacman

## TODO

- [ ] More flexible buildsystem (still plain makefiles but better)
  - [ ] `make install`
  - [ ] `make uninstall`
  - [ ] maybe split up library in multiple files at some point, rather than
        having all helpers in the same c file
- [ ] `portf -Ss`
- [ ] `portf -Sy`
- [ ] `portf -Q` query local db
- [ ] configuration system
  - [ ] Like we need to be able to configure multiple apis, so we need to be
        able to have multiple api keys
  - [ ] Region
  - [ ] Currency, not everybody live in the US.
  - [ ] (Daemon) Define market times only sync between
    - [ ] (Daemon) 9:30am - 4pm (US, NY)
    - [ ] (Daemon) 8am - 10pm (Germany)
    - And more should be possible
  - [ ] (Daemon) define refresh time
- [ ] Watchers, should also be handle in configuration file
  - [ ] How much scripting do i want to provide or just constraints
    - stonk[aapl].price < 100 (watches pre, regular and post price)
    - stonk[aapl].regular.price < 100 (watches regular)
    - port[gme].change_percent > 10 (watches changes in percent only if
      gme in a portfolio)
  - [ ] Its difficult to think about this, it would be cool to be able to
        script more advanced things here but i also don't want to embed `lua`
        into the library(core).

## Installation

Again don't use it but if you want you need following things:

1. API Key for yahoo finance low latency API in `.config/portf`
  - this might change in the future
2. `make`
  - Make sure you have the dependencies installed. Currently libcurl and cjson
3. `export LD_LIBRARY_PATH=$PWD/build:$LD_LIBRARY_PATH` so it finds the .so
4. `./build/portf -Sf gme amc aapl`
  - i am still designing the cli, so the call might change in the future
5. in neovim (assumining cwd = this repo)
  - `:lua require('lua/portf/init').get_share({ 'GME', 'AMC', 'AAPL' })`
  - subject to change

(currently only linux support, might change in the future)

## Bad jokes

`:%s/stonks/stocks/g`
