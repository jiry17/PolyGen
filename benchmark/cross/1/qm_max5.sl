(set-logic LIA)

(define-fun qm ((a Int) (b Int)) Int
      (ite (< a 0) b a))

(synth-fun qm-foo ((v Int) (w Int) (x Int) (y Int) (z Int) ) Int
    ((Start Int (v
    			 w
    			 x
    			 y
    			 z
                 0
                 1
                 (- Start Start)
                 (+ Start Start)
                 (qm Start Start)))))

(declare-var v Int)
(declare-var w Int)
(declare-var x Int)
(declare-var y Int)
(declare-var z Int)


(constraint (= (qm-foo v w x y z) 
                (ite (and (and (and (>= v w ) (>= v x)) (>= v y)) (>= v z)) v
                  (ite (and (and (>= w x) (>= w y)) (>= w z)) w
                    (ite (and (>= x y) (>= x z)) x 
                      (ite (>= y z) y z) ) ) ) ) )

(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int)) Int (ite (and (and (and (< Param3 Param0) (< Param1 Param0)) (<= Param4 Param0)) (<= Param2 Param0)) Param0 (ite (and (and (< Param4 Param1) (<= Param3 Param1)) (<= Param2 Param1)) Param1 (ite (and (< Param4 Param2) (<= Param3 Param2)) Param2 (ite (<= Param4 Param3) Param3 Param4)))))