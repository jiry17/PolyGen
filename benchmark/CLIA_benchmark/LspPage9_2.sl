(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lsum Int) (rsum Int)) Int
    ((Start Int (lans
                 rans
                 lmin
                 rmin
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


(define-fun ans ((lans Int) (rans Int) (lsum Int) (rsum Int)) Int
  (ite (<= (+ lsum rsum) 10)
       1
       0))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(constraint (= (ans a b c d) (combine a b c d)))

(check-synth)

