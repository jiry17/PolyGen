(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lhead1s Int) (rhead1s Int) (ltail1s Int) (rtail1s Int)) Int
    ((Start Int (lans
                 rans
                 lhead1s
                 rhead1s
                 ltail1s
                 rtail1s
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

(define-fun max2 ((x Int) (y Int)) Int 
  (ite (>= x y) x y)
)

(define-fun ans ((lans Int) (rans Int) (lhead1s Int) (rhead1s Int) (ltail1s Int) (rtail1s Int)) Int
  (max2 (max2 lans rans) (+ ltail1s rhead1s)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (= (ans a b c d e f) (combine a b c d e f)))

(check-synth)

