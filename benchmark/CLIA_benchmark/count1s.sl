(set-logic LIA)

(synth-fun combine ((lans Int) (rans Int) (lhead Int) (rhead Int) (ltail Int) (rtail Int)) Int
    ((Start Int (lans
                 rans
                 lhead
                 rhead
                 ltail
                 rtail
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


(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))

(define-fun ans ((lans Int) (rans Int) (lhead Int2) (rhead Int2) (ltail Int2) (rtail Int2)) Int
  (ite (and (= 1 ltail) (= 1 rhead))
       (- (+ lans rans) 1)
       (+ lans rans)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int2)
(declare-var d Int2)
(declare-var e Int2)
(declare-var f Int2)
(constraint (=> (and (and (ib c) (ib d)) (and (ib e) (ib f))) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

