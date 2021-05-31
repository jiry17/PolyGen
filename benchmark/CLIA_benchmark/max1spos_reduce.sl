(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lmax1s Int) (rmax1s Int) (rhead1s Int) (ltail1s Int) (llen Int)) Int
    ((Start Int (lans
                 rans
                 lmax1s
                 rmax1s
                 rhead1s
                 ltail1s
                 llen
                 0
                 1
                 (+ Start Start)
                 (- Start Start)
                 (ite StartBool Start Start)))
     (StartBool Bool ((and StartBool StartBool)
                      (or  StartBool StartBool)
                      (not StartBool)
                      (<=  Start Start)
                      (=   Start Start)
                      (>=  Start Start)))))



(define-fun ans  ((lans Int) (rans Int) (lmax1s Int) (rmax1s Int) (rhead1s Int) (ltail1s Int) (llen Int)) Int
  (ite (>= lmax1s (+ ltail1s rhead1s))
       (ite (>= lmax1s rmax1s)
            lans
            (+ rans llen))
       (ite (>= (+ ltail1s rhead1s) rmax1s)
            (- llen ltail1s)
            (+ rans llen))))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(declare-var g Int)
(constraint (= (ans a b c d e f g) (combine a b c d e f g)))

(check-synth)

