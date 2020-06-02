:set -XOverloadedStrings
:set prompt ""
:set prompt-cont ""

import Sound.Tidal.Context

-- CineVivo Target
:{
cvTarget = Target {oName     = "CineVivo",
                   oAddress  = "127.0.0.1",
                   oPort     = 8000,
                   oLatency  = 0.2,
                   oWindow   = Nothing,
                   oSchedule = Pre BundleStamp
                  }
:}

-- CineVivo OSC Specs
:{
cvOSCSpecs = [(OSC "/tidalVideo" $ ArgList [("video",          Nothing),
                                          ("vbegin",         Just $ VF 0),
                                          ("vend",           Just $ VF 1),
                                          ("vloop",          Just $ VI 0),
                                          ("vspeed",         Just $ VF 1),
                                          ("vgain",          Just $ VF 0),
                                          ("valpha",         Just $ VF 1),
                                          ("vlayer",         Just $ VI 0),
                                          ("vwidth",         Just $ VF 1),
                                          ("vheight",        Just $ VF 1),
                                          ("vposx",          Just $ VF 0),
                                          ("vposy",          Just $ VF 0),
                                          ("vred",           Just $ VF 1),
                                          ("vgreen",         Just $ VF 1),
                                          ("vblue",          Just $ VF 1),
                                          ("vrotationangle", Just $ VF 0),
                                          ("vrotationspeed", Just $ VF 0),
                                          ("vrotationaxis",  Just $ VS "x"),
                                          ("delta",          Just $ VF 0),
                                          ("cycle",          Just $ VF 0),
                                          ("cps",            Just $ VF 0)])]
:}

-- CineVivo pattern parameters
:{
let video     = pS "video"          -- video                 pattern string 0
    vbegin    = pF "vbegin"         -- video begin           pattern float  1
    vend      = pF "vend"           -- video end             pattern float  2
    vloop     = pI "vloop"          -- video loop            pattern bool   3
    vspeed    = pF "vspeed"         -- video speed           pattern float  4
    vgain     = pF "vgain"          -- video audio gain      pattern float  5
    valpha    = pF "valpha"         -- video alpha channel   pattern float  6
    vlayer    = pI "vlayer"         -- video layer           pattern int    7
    vwidth    = pF "vwidth"         -- video width           pattern float  8
    vheight   = pF "vheight"        -- video height          pattern float  9
    vposx     = pF "vposx"          -- video position X      pattern float  10
    vposy     = pF "vposy"          -- video position Y      pattern float  11
    vred      = pF "vred"           -- video colour red      pattern float  12
    vgreen    = pF "vgreen"         -- video colour green    pattern float  13
    vblue     = pF "vblue"          -- video colour blue     pattern float  14
    vrotangle = pF "vrotationangle" -- video rotation angle  pattern float  15
    vrotspeed = pF "vrotationspeed" -- video rotation speed  pattern float  16
    vrotaxis  = pS "vrotationaxis"  -- video rotation axis   pattern string 17
:}

-- CineVivo pattern parameter shorthands. Example:
-- ```
-- cv1
--   $ v     "vid"   -- video file
--   # vs    1       -- speed
--   # vio   0 1     -- in-out / begin-end
--   # vrgb  1 1 1   -- colour
--   # vxywh 0 0 1 1 -- x, y, width & height
--   # vrot  0 1 "x" -- rotation angle, speed & axis
--   # val   1 0     -- alpha & layer
-- ```
:{
let v  = video
    vs = vspeed
    vi = vbegin
    vo = vend
    vw = vwidth
    vh = vheight
    vx = vposx
    vy = vposy
    vr = vred
    vg = vgreen
    vb = vblue
    va = valpha
    vl = vlayer
    vrota = vrotangle
    vrots = vrotspeed
    vrotx = vrotaxis
    vxy    x     y       = vx x # vy y                 -- video position XY
    vxy'   x (#) y       = vx x # vy y                 -- video position XY, with operator
    vwh    w     h       = vw w # vh h                 -- video scale width & height
    vwh'   w (#) h       = vw w # vh h                 -- video scale width & height, with operator
    vxywh  x  y     w  h = vxy x y # vwh w h           -- video position XY & video scale width & height
    vxywh' x  y (#) w  h = vxy x y # vwh w h           -- video position XY & video scale width & height, with operator
    vrgb   r     g     b = vr r # vg g # vb b          -- video colour red green blue
    vrgb'  r (#) g (+) b = vr r # vg g + vb b          -- video colour red green blue, with operators
    vio    i     o       = vi i # vo o                 -- video in/out
    vio'   i (#) o       = vi i # vo o                 -- video in/out, with operator
    val    a     l       = va a # vl l                 -- video alpha & layer
    val'   a (#) l       = va a # vl l                 -- video alpha & layer, with operator
    vrot   a     s     x = vrota a # vrots s # vrotx x -- video rotation
    vrot'  a (#) s (+) x = vrota a # vrots s + vrotx x -- video rotation, with operator
    vxywh'' x (#) y (+) w (-) h = vx x # vy y + vw w - vh h -- video position XY & video scale width & height, with operators
:}

-- CineVivo OSC Map
-- uncomment `(superdirtTarget, [superdirtShape])` to enable sending to SuperDirt in the same pattern
:{
cvOscMap = [--(superdirtTarget, [superdirtShape]),
            (cvTarget, cvOSCSpecs)]
:}

-- CineVivo Stream
cvStream <- startStream defaultConfig {cFrameTimespan = 1/20} cvOscMap

-- CineVivo Stream Controls
:{
let cv          = streamReplace cvStream
    hush        = streamHush cvStream
    list        = streamList cvStream
    mute        = streamMute cvStream
    unmute      = streamUnmute cvStream
    solo        = streamSolo cvStream
    unsolo      = streamUnsolo cvStream
    once        = streamOnce cvStream
    first       = streamFirst cvStream
    asap        = once
    nudgeAll    = streamNudgeAll cvStream
    all         = streamAll cvStream
    resetCycles = streamResetCycles cvStream
    setcps      = asap . cps
    xfade i     = transition cvStream True (Sound.Tidal.Transition.xfadeIn 4) i
    xfadeIn i t = transition cvStream True (Sound.Tidal.Transition.xfadeIn t) i
    histpan i t = transition cvStream True (Sound.Tidal.Transition.histpan t) i
    wait i t    = transition cvStream True (Sound.Tidal.Transition.wait t) i
    waitT i f t = transition cvStream True (Sound.Tidal.Transition.waitT f t) i
    jump i      = transition cvStream True (Sound.Tidal.Transition.jump) i
    jumpIn i t  = transition cvStream True (Sound.Tidal.Transition.jumpIn t) i
    jumpIn' i t = transition cvStream True (Sound.Tidal.Transition.jumpIn' t) i
    jumpMod i t = transition cvStream True (Sound.Tidal.Transition.jumpMod t) i
    interpolate i     = transition cvStream True (Sound.Tidal.Transition.interpolate) i
    interpolateIn i t = transition cvStream True (Sound.Tidal.Transition.interpolateIn t) i
    clutch i          = transition cvStream True (Sound.Tidal.Transition.clutch) i
    clutchIn i t      = transition cvStream True (Sound.Tidal.Transition.clutchIn t) i
    anticipate i      = transition cvStream True (Sound.Tidal.Transition.anticipate) i
    anticipateIn i t  = transition cvStream True (Sound.Tidal.Transition.anticipateIn t) i
    forId i t         = transition cvStream False (Sound.Tidal.Transition.mortalOverlay t) i
    mortal i lifespan release = transition cvStream True (Sound.Tidal.Transition.mortal lifespan release) i
    cv1  = cv 1 . (|< orbit 0)
    cv2  = cv 2 . (|< orbit 1)
    cv3  = cv 3 . (|< orbit 2)
    cv4  = cv 4 . (|< orbit 3)
    cv5  = cv 5 . (|< orbit 4)
    cv6  = cv 6 . (|< orbit 5)
    cv7  = cv 7 . (|< orbit 6)
    cv8  = cv 8 . (|< orbit 7)
    cv9  = cv 9 . (|< orbit 8)
    cv10 = cv 10 . (|< orbit 9)
    cv11 = cv 11 . (|< orbit 10)
    cv12 = cv 12 . (|< orbit 11)
    cv13 = cv 13
    cv14 = cv 14
    cv15 = cv 15
    cv16 = cv 16
:}

-- CineVivo shared variables
:{
let setI = streamSetI cvStream
    setF = streamSetF cvStream
    setS = streamSetS cvStream
    setR = streamSetR cvStream
    setB = streamSetB cvStream
:}

-- :set prompt "~ "
