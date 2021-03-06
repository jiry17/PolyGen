(set-logic LIA)
(synth-fun findIdx ( (y1 Int) (y2 Int) (y3 Int) (y4 Int) (y5 Int) (y6 Int) (y7 Int) (y8 Int) (y9 Int) (y10 Int) (k1 Int)) Int ((Start Int ( 0 1 2 3 4 5 6 7 8 9 10 y1 y2 y3 y4 y5 y6 y7 y8 y9 y10 k1 (ite BoolExpr Start Start))) (BoolExpr Bool ((< Start Start) (<= Start Start) (> Start Start) (>= Start Start)))))
(declare-var x1 Int)
(declare-var x2 Int)
(declare-var x3 Int)
(declare-var x4 Int)
(declare-var x5 Int)
(declare-var x6 Int)
(declare-var x7 Int)
(declare-var x8 Int)
(declare-var x9 Int)
(declare-var x10 Int)
(declare-var k Int)
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (< k x1) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 0))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (> k x10) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 10))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x1) (< k x2)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 1))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x2) (< k x3)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 2))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x3) (< k x4)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 3))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x4) (< k x5)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 4))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x5) (< k x6)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 5))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x6) (< k x7)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 6))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x7) (< k x8)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 7))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x8) (< k x9)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 8))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (< x9 x10))))))))) (=> (and (> k x9) (< k x10)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 k) 9))))
(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int) (Param6 Int) (Param7 Int) (Param8 Int) (Param9 Int) (Param10 Int)) Int (ite (<= Param9 Param10) 10 (ite (<= Param8 Param10) 9 (ite (<= Param7 Param10) 8 (ite (<= Param6 Param10) 7 (ite (<= Param5 Param10) 6 (ite (<= Param4 Param10) 5 (ite (<= Param3 Param10) 4 (ite (<= Param2 Param10) 3 (ite (<= Param1 Param10) 2 (ite (and (<= Param10 Param1) (<= Param0 Param10)) 1 0)))))))))))