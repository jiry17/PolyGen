(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lmax1s Int) (rmax1s Int) (lhead1s Int) (rhead1s Int) (ltail1s Int) (rtail1s Int) (llen Int) (rlen Int)) Int
    ((Start Int (lans
                 rans
                 lmax1s
                 rmax1s
                 lhead1s
                 rhead1s
                 ltail1s
                 rtail1s
                 llen
                 rlen
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



(define-fun ans  ((lans Int) (rans Int) (lmax1s Int) (rmax1s Int) (lhead1s Int) (rhead1s Int) (ltail1s Int) (rtail1s Int) (llen Int) (rlen Int)) Int
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
(declare-var h Int)
(declare-var i Int)
(declare-var j Int)
(constraint (= (ans a b c d e f g h i j) (combine a b c d e f g h i j)))

(check-synth)

