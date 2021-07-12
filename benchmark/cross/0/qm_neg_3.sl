(set-logic LIA)

(define-fun qm ((a Int) (b Int)) Int
      (ite (< a 0) b a))

(synth-fun qm-foo ((x Int) (y Int) (z Int)) Int
    ((Start Int (x
                 y
                 z
                 0
                 1
                 (- Start Start)
                 (qm Start Start)))))

(declare-var x Int)
(declare-var y Int)
(declare-var z Int)

(constraint (= (qm-foo x y z)  
                (ite (and (< x 0) (and (< y 0) (< z 0))) 1 0))) 

(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int)) Int (ite (and (and (< Param2 0) (< Param1 0)) (< Param0 0)) 1 0))