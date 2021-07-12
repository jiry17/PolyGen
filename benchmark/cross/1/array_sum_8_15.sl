(set-logic LIA)
(synth-fun findSum ( (y1 Int) (y2 Int) (y3 Int) (y4 Int) (y5 Int) (y6 Int) (y7 Int) (y8 Int) )Int ((Start Int ( 0 1 2 3 4 5 y1 y2 y3 y4 y5 y6 y7 y8 z (+ Start Start)  (let ((z Int Start)) Start)  (ite BoolExpr Start Start))) (BoolExpr Bool ((< Start Start) (<= Start Start) (> Start Start) (>= Start Start)))))
(declare-var x1 Int)
(declare-var x2 Int)
(declare-var x3 Int)
(declare-var x4 Int)
(declare-var x5 Int)
(declare-var x6 Int)
(declare-var x7 Int)
(declare-var x8 Int)
(constraint (=> (> (+ x1 x2) 15) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x1 x2))))
(constraint (=> (and (<= (+ x1 x2) 15) (> (+ x2 x3) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x2 x3))))
(constraint (=> (and (and (<= (+ x1 x2) 15) (<= (+ x2 x3) 15)) (> (+ x3 x4) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x3 x4))))
(constraint (=> (and (and (<= (+ x1 x2) 15) (and (<= (+ x2 x3) 15) (<= (+ x3 x4) 15))) (> (+ x4 x5) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x4 x5))))
(constraint (=> (and (and (<= (+ x1 x2) 15) (and (<= (+ x2 x3) 15) (and (<= (+ x3 x4) 15) (<= (+ x4 x5) 15)))) (> (+ x5 x6) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x5 x6))))
(constraint (=> (and (and (<= (+ x1 x2) 15) (and (<= (+ x2 x3) 15) (and (<= (+ x3 x4) 15) (and (<= (+ x4 x5) 15) (<= (+ x5 x6) 15))))) (> (+ x6 x7) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x6 x7))))
(constraint (=> (and (and (<= (+ x1 x2) 15) (and (<= (+ x2 x3) 15) (and (<= (+ x3 x4) 15) (and (<= (+ x4 x5) 15) (and (<= (+ x5 x6) 15) (<= (+ x6 x7) 15)))))) (> (+ x7 x8) 15)) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) (+ x7 x8))))
(constraint (=> (and (<= (+ x1 x2) 15) (and (<= (+ x2 x3) 15) (and (<= (+ x3 x4) 15) (and (<= (+ x4 x5) 15) (and (<= (+ x5 x6) 15) (and (<= (+ x6 x7) 15) (<= (+ x7 x8) 15))))))) (= (findSum x1 x2 x3 x4 x5 x6 x7 x8 ) 0)))
(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int) (Param6 Int) (Param7 Int)) Int (ite (< 15 (+ Param0 Param1)) (+ Param0 Param1) (ite (< 15 (+ Param1 Param2)) (+ Param1 Param2) (ite (< 15 (+ Param2 Param3)) (+ Param2 Param3) (ite (< 15 (+ Param3 Param4)) (+ Param3 Param4) (ite (< 15 (+ Param4 Param5)) (+ Param4 Param5) (ite (< 15 (+ Param5 Param6)) (+ Param5 Param6) (ite (< 15 (+ Param6 Param7)) (+ Param6 Param7) 0))))))))