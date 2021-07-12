(set-logic LIA)

(define-fun qm ((a Int) (b Int)) Int
      (ite (< a 0) b a))

(synth-fun qm-foo ((v Int) (w Int) (x Int) (y Int) (z Int)) Int
    ((Start Int (v
    			 w
    			 x
                 y
                 z
                 0
                 1
                 (- Start Start)
                 (qm Start Start)))))

(declare-var v Int)
(declare-var w Int)
(declare-var x Int)
(declare-var y Int)
(declare-var z Int)

(constraint (= (qm-foo v w x y z)  
                (ite (and (<= v 0) (and (<= w 0) (and (<= x 0) (and (<= y 0) (<= z 0))))) 1 0))) 

(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int)) Int (ite (and (and (and (and (<= Param4 0) (<= Param3 0)) (<= Param2 0)) (<= Param1 0)) (<= Param0 0)) 1 0))