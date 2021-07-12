(set-logic LIA)
(synth-fun findIdx ( (y1 Int) (y2 Int) (y3 Int) (y4 Int) (y5 Int) (y6 Int) (y7 Int) (y8 Int) (y9 Int) (y10 Int) (y11 Int) (y12 Int) (y13 Int) (k1 Int)) Int ((Start Int ( 0 1 2 3 4 5 6 7 8 9 10 11 12 13 y1 y2 y3 y4 y5 y6 y7 y8 y9 y10 y11 y12 y13 k1 (ite BoolExpr Start Start))) (BoolExpr Bool ((< Start Start) (<= Start Start) (> Start Start) (>= Start Start)))))
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
(declare-var x11 Int)
(declare-var x12 Int)
(declare-var x13 Int)
(declare-var k Int)
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (< k x1) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 0))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (> k x13) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 13))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x1) (< k x2)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 1))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x2) (< k x3)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 2))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x3) (< k x4)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 3))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x4) (< k x5)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 4))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x5) (< k x6)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 5))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x6) (< k x7)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 6))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x7) (< k x8)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 7))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x8) (< k x9)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 8))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x9) (< k x10)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 9))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x10) (< k x11)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 10))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x11) (< k x12)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 11))))
(constraint (=> (and (< x1 x2) (and (< x2 x3) (and (< x3 x4) (and (< x4 x5) (and (< x5 x6) (and (< x6 x7) (and (< x7 x8) (and (< x8 x9) (and (< x9 x10) (and (< x10 x11) (and (< x11 x12) (< x12 x13)))))))))))) (=> (and (> k x12) (< k x13)) (= (findIdx x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 k) 12))))
(check-synth)
(define-fun res ((Param0 Int) (Param1 Int) (Param2 Int) (Param3 Int) (Param4 Int) (Param5 Int) (Param6 Int) (Param7 Int) (Param8 Int) (Param9 Int) (Param10 Int) (Param11 Int) (Param12 Int) (Param13 Int)) Int (ite (<= Param12 Param13) 13 (ite (<= Param11 Param13) 12 (ite (<= Param10 Param13) 11 (ite (<= Param9 Param13) 10 (ite (<= Param8 Param13) 9 (ite (<= Param7 Param13) 8 (ite (<= Param6 Param13) 7 (ite (<= Param5 Param13) 6 (ite (<= Param4 Param13) 5 (ite (<= Param3 Param13) 4 (ite (<= Param2 Param13) 3 (ite (<= Param1 Param13) 2 (ite (and (<= Param13 Param1) (<= Param0 Param13)) 1 0))))))))))))))