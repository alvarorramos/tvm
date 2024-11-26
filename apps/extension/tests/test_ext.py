# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
import tvm_ext
import tvm
import numpy as np

def test_bind_add():
    def add(a, b):
        return a + b
    f = tvm_ext.bind_add(add, 1)
    assert f(2)  == 3

def test_ext_dev():
    n = 10
    A = tvm.placeholder((n,), name='A')
    B = tvm.compute((n,), lambda *i: A(*i) + 1.0, name='B')
    s = tvm.create_schedule(B.op)
    def check_llvm():
        if not tvm.module.enabled("llvm"):
            return
        f = tvm.build(s, [A, B], "ext_dev", "llvm")
        device = tvm.ext_dev(0)
        # launch the kernel.
        a = tvm.nd.array(np.random.uniform(size=n).astype(A.dtype), device)
        b = tvm.nd.array(np.zeros(n, dtype=B.dtype), device)
        f(a, b)
        tvm.testing.assert_allclose(b.asnumpy(), a.asnumpy() + 1)
    check_llvm()


def test_sym_add():
    a = tvm.var('a')
    b = tvm.var('b')
    c = tvm_ext.sym_add(a, b)
    assert c.a == a and c.b == b


def test_ext_vec():
    ivec = tvm_ext.ivec_create(1, 2, 3)
    assert(isinstance(ivec, tvm_ext.IntVec))
    assert ivec[0] == 1
    assert ivec[1] == 2

    def ivec_cb(v2):
        assert(isinstance(v2, tvm_ext.IntVec))
        assert v2[2] == 3

    tvm.convert(ivec_cb)(ivec)


def test_extract_ext():
    fdict = tvm.extract_ext_funcs(tvm_ext._LIB.TVMExtDeclare)
    assert fdict["mul"](3, 4) == 12


def test_extern_call():
    n = 10
    A = tvm.placeholder((n,), name='A')
    B = tvm.compute((n,), lambda *i: tvm.call_extern("float32", "TVMTestAddOne", A(*i)), name='B')
    s = tvm.create_schedule(B.op)

    def check_llvm():
        if not tvm.module.enabled("llvm"):
            return
        f = tvm.build(s, [A, B], "llvm")
        device = tvm.cpu(0)
        # launch the kernel.
        a = tvm.nd.array(np.random.uniform(size=n).astype(A.dtype), device)
        b = tvm.nd.array(np.zeros(n, dtype=B.dtype), device)
        f(a, b)
        tvm.testing.assert_allclose(b.asnumpy(), a.asnumpy() + 1)
    check_llvm()


def test_nd_subclass():
    a = tvm_ext.NDSubClass.create(addtional_info=3)
    b = tvm_ext.NDSubClass.create(addtional_info=5)
    c = a + b
    d = a + a
    e = b + b
    assert(a.addtional_info == 3)
    assert(b.addtional_info == 5)
    assert(c.addtional_info == 8)
    assert(d.addtional_info == 6)
    assert(e.addtional_info == 10)


if __name__ == "__main__":
    test_nd_subclass()
    test_extern_call()
    test_ext_dev()
    test_ext_vec()
    test_bind_add()
    test_sym_add()
    test_extract_ext()
