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

(define-fun ans ((lans Int) (rans Int) (lhead Int) (rhead Int) (ltail Int) (rtail Int)) Int
  (ite (and (= 1 ltail) (= 1 rhead))
       (- (+ lans rans) 1)
       (+ lans rans)))


(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (and (ib c) (ib d)) (and (ib e) (ib f))) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int)) Int (ite (or (<= Param3 0) (<= Param4 0)) (+ Param0 Param1) (+ (+ Param0 Param1) -1)))