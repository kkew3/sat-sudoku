from libcpp.vector cimport vector


cdef extern from "c_enc.hpp":
    cdef cppclass VariableManager:
        VariableManager()
        vector[vector[int]] convert(vector[vector[int]] cclauses)
        vector[int] back(vector[int] solution)
