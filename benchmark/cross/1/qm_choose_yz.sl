(set-logic LIA)

(define-fun qm ((a Int) (b Int)) Int
      (ite (< a 0) b a))

(synth-fun qm-choose ((x Int) (y Int) (z Int) ) Int
    ((Start Int (x
                 y
                 z
                 0
                 1
                 (- Start Start)
                 (+ Start Start)
                 (qm Start Start)))))

(declare-var x Int)
(declare-var y Int)
(declare-var z Int)

(constraint (= (qm-choose x y z) (ite (<= x 0) y z))) 

(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int)) Int (ite (<= Param0 0) Param1 Param2))