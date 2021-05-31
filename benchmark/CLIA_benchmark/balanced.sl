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

(declare-var a Int2)
(declare-var b Int2)
(declare-var c Int)
(declare-var d Int)
(declare-var e Int)
(declare-var f Int)
(constraint (=> (and (ib a) (ib b)) (= (ans a b c d e f) (combine a b c d e f))))

(check-synth)

