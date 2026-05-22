// Copyright, 2024. See LICENSE for copyright holder information.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// (3) Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#pragma once

#include "precision.hh"
#include "dimensions.hh"
#include "device.hh"

namespace RandBLAS {

enum Layout { ROW_MAJOR, COL_MAJOR };
enum class MatrixType : std::size_t { DENSE = 0, CSR = 1, Count};

// Matrix storage types used by MatrixInternals to store the actual matrix data. These are used for dynamic dispatch of the correct apply call in sketch_general.

struct DenseStorage {
    NumericPtrVariant values_;
    Layout layout_ = COL_MAJOR;
    size_t lead_dim_ = 0;
    // DenseStorage(size_t lead_dim, Layout layout) : lead_dim_{lead_dim}, layout_{layout} {}
};  // struct DenseStorage
 
struct CsrStorage {
    size_t nnz_;
    NumericPtrVariant values_;
    IntPtrVariant row_ptrs_;
    IntPtrVariant col_idxs_;
    // CsrStorage(size_t nnz) : nnz_{nnz} {}
};  // struct CsrStorage

using StorageVariant = std::variant<DenseStorage, CsrStorage>;

// MatrixInternals is the internal representation of data and metadata for a matrix.

struct MatrixInternals {
public:
    Device device_;
    size_t num_elems = 0;
    dim<2> size_     = {0, 0};
    NumericType store_precision;
    IntType     index_precision;
    MatrixType matrix_type;

    // holds contiguously allocated memory for storing the matrix.
    StorageVariant storage;

    // MatrixInternalsDispatcher method_dispatcher_;

    MatrixInternals() = default;
    MatrixInternals(dim<2> size, MatrixType Matrix, Device device, NumericType store_precision);
    MatrixInternals(dim<2> size, size_t nnz, MatrixType Matrix, Device device, NumericType store_precision);

    // @TODO: have to define the MatrixDescriptor object first.
    // MatrixInternals(MatrixDescriptor descriptor, Device device, NumericType store_precision);

    ~MatrixInternals();

    void copy_from(MatrixInternals& c);

};  // struct MatrixInternals

// Matrix object that users interact with. It holds a MatrixInternals object and provides the API for users to interact with the matrix.

struct DenseVIew;
struct CsrView;

struct Matrix {
    friend struct DenseView;
    friend struct CsrView;

public:    
    // @TODO: add create functions

    // @TODO: add copy_from function

    // @TODO: add getters/setters for size, type, device, etc that makes sense to be exposed to users.
protected:

    // @TODO: add constructors

private:
    MatrixInternals internals_;    
};

// View objects for dynamic dispatch of the correct apply call in sketch_general.
// A view object will perform a shallow copy of the relevant metadata and data pointers from the Matrix object, and this view object will be passed to the apply call in sketch_general.

struct DenseView {
    dim<2> size_;
    Layout layout_ = COL_MAJOR;
    NumericPtrVariant values_;
    Device device_;
    size_t lead_dim_ = 0;
    DenseView(Matrix& source) :
        size_(source.internals_.size_),
        lead_dim_(std::get<DenseStorage>(source.internals_.storage).lead_dim_),
        layout_(std::get<DenseStorage>(source.internals_.storage).layout_),
        values_(std::get<DenseStorage>(source.internals_.storage).values_),
        device_(source.internals_.device_) {}
};  // DenseView

struct CsrView {
    dim<2> size_;
    NumericPtrVariant values_;
    Device device_;
    size_t nnz_ = 0;
    CsrView(Matrix& source) :
        size_(source.internals_.size_),
        nnz_(std::get<CsrStorage>(source.internals_.storage).nnz_),
        values_(std::get<CsrStorage>(source.internals_.storage).values_),
        device_(source.internals_.device_) {}
};  // CsrView


} // namespace RandBLAS
