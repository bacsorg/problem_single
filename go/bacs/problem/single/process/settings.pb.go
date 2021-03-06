// Code generated by protoc-gen-go.
// source: bacs/problem/single/process/settings.proto
// DO NOT EDIT!

package process

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import bacs_process "github.com/bacsorg/common/go/bacs/process"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

type Settings struct {
	ResourceLimits *bacs_process.ResourceLimits `protobuf:"bytes,1,opt,name=resource_limits" json:"resource_limits,omitempty"`
	File           []*File                      `protobuf:"bytes,2,rep,name=file" json:"file,omitempty"`
	Execution      *Execution                   `protobuf:"bytes,3,opt,name=execution" json:"execution,omitempty"`
}

func (m *Settings) Reset()         { *m = Settings{} }
func (m *Settings) String() string { return proto.CompactTextString(m) }
func (*Settings) ProtoMessage()    {}

func (m *Settings) GetResourceLimits() *bacs_process.ResourceLimits {
	if m != nil {
		return m.ResourceLimits
	}
	return nil
}

func (m *Settings) GetFile() []*File {
	if m != nil {
		return m.File
	}
	return nil
}

func (m *Settings) GetExecution() *Execution {
	if m != nil {
		return m.Execution
	}
	return nil
}
