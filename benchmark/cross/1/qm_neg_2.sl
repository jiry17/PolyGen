(set-logic LIA)

(define-fun qm ((a Int) (b Int)) Int
      (ite (< a 0) b a))

(synth-fun qm-foo ((x Int) (y Int) ) Int
    ((Start Int (x
                 y
                 0
                 1
                 (- Start Start)
                 (qm Start Start)))))

(declare-var x Int)
(declare-var y Int)

(constraint (= (qm-foo x y)  
                (ite (and (< x 0)  (< y 0) ) 1 0))) 

(check-synth)
(define-fun res ((Param0 Int) (Param1 Int)) Int (ite (and (< Param1 0) (< Param0 0)) 1 0))