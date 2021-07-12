(set-logic LIA)

(synth-fun combine ((lbalance Int) (rbalance Int) (lsum Int) (rsum Int) (lminps Int) (rminps Int)) Int
    ((Start Int (lbalance
                 rbalance
                 lsum
                 rsum
                 lminps
                 rminps
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



(define-fun ans ((lbalance2 Int) (rbalance2 Int) (lsum Int) (rsum Int) (lminps Int) (rminps Int)) Int
  (ite (= 0 (+ lsum rsum))
       (ite (>= lminps 0)
            (ite (>= (+ lsum rminps) 0)
                 1
                 0)
            0)
       0))

(define-fun ib ((x Int)) Bool (and (>= x 0) (<= x 1)))

(declare-var a Int)
(declare-var b Int)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (ib a) (ib b)) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int)) Int (ite (and (and (and (<= (+ Param2 Param3) 0) (<= (+ Param2 Param3) Param4)) (<= 0 (+ Param2 Param3))) (<= Param3 Param5)) 1 0))