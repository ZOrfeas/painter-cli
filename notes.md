### Notes

Implemented functionality:
- subcommands
- local/global flags/options
    - flags (boolean, toggle)
    - options (non-boolean, set)
- arguments passed to command actions (anything that is not a flag name, an option name, an option value or a subcommand)

implementation_details:  
**x: any string of length 1**,  
**y: any string of any length (>=0)**
- tokens
    - -x, --y, -y (long/short form options and flags. e.g. -l, --long, -l1000)
    - y
        - value of option (e.g. -l 1000)
        - name of subcommand (e.g. node, subnode, bootstrap, or any other non-flag/option name)
        - value of raw argument to pass to command action  
    
    real tokens do not provide enough detail to be used as events for the fsm.  
    therefore, we need some-way to split them into more categories
    | token-form | description                                                | token-name |
    | ---------- | ---------------------------------------------------------- | ---------- |
    | -x         | short flag/option                                          | sfl / sopt |
    | -y -> -x y | short option and value (!not flag since a value is given!) | sfl val    |
    | --y        | long flag/option                                           | lfl / lopt |
    | y          | option value                                               | val        |
    | y          | subcommand name                                            | cmdname    |
    | y          | raw argument to pass                                       | rawarg     |

    remaining issues:
    - decide between (s/l)fl and (s/l)opt (doable by keeping track of visible flags/options and seeing which one they are)
    - decide between val cmdname rawarg
      - if at state after (s/l)opt and y is not a visible cmdname, then it's a val
      - if at state after (s/l)opt and y is a visible cmdname, then error 'conflict'
      - if at any other state and y is a visible cmdname, then cmdname
      - else rawarg

- decisions
    - flags/options not consumed will produce an error



_notes_:
- each flag name can only exist once. (this allows for flags/options to be grabbed from any place in the arguments)