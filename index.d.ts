// Type definitions for hdf5 0.3
// Project: https://github.com/HDF-NI/hdf5.node#readme
// Definitions by: NINI1988 <https://github.com/NINI1988>

import { Access, H5OType, HLType, H5Type, H5TOrder, H5SType } from "hdf5/lib/globals";

/// <reference types="node" />

type TypedArray = Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Uint8ClampedArray | Float32Array | Float64Array;

type TableArray = TableTypedArray | TableStringArray | TableNumberArray;

type TableTypedArray = TypedArray & { name?: string };

type TableStringArray = { name?: string, [key: number]: string };

type TableNumberArray = { name?: string, [key: number]: number, type?: H5Type };

type Hdf5Id = number;

export declare class Hdf5Buffer extends Buffer {
    rank?: number;
    type?: H5Type;

    /**
     * dimension 1
     */
    rows?: number;

    /**
     * dimension 2
     */
    columns?: number;

    /**
     * dimension 3
     */
    sections?: number;

    /**
     * dimension 4
     */
    files?: number;
}

export declare namespace hdf5 {

    class Methods {

        public id: Hdf5Id;

        /**
         * 
         * @param xpath  the path to the group to open. Delimited with / and will throw an exception if it doesn’t exist. Useful particularly in read only mode
         * @returns the opened group object
         */
        public openGroup(xpath: string): Group;

        /**
         * Refreshes subtree and metadata from h5 file to h5 components in memory and on to the javascript. Reverse action to flush.
         */
        public refresh(): void;

        /**
         * 
         * @param source the name of the source group
         * @param id the group or file id location. Every group and the file return with an id attribute. In this case it is the destination’s future location. If it is the file’s id this essentially renames the source
         * @param destination the name of the destination group.
         */
        public move(source: string, id: Hdf5Id, destination: string): any;

        /**
         * @returns # of attributes
         */
        public getNumAttrs(): number;

        /**
         * 
         * @param name  the name of the group to delete. An apprehensive activity with an h5 file but sometimes it is necessary
         */
        public delete(name: string): void;

        /**
         * Flushes subtree and metadata from javascript to h5 components in memory and on to the h5 file.
         */
        public flush(): void;

        public close(): void;

        /**
         * @returns # of children
         */
        public getNumObjs(): number;

        /**
         * @returns an array with the names in alphabetic order
         */
        public getMemberNames(): string[];

        /**
         * @returns an array with the names in creation order
         */
        public getMemberNamesByCreationOrder(): string[];

        /**
         * @param name the name of the child
         * @returns the type according to H5OType and indicates group, dataset or named datatype
         */
        public getChildType(name: string): H5OType;

        /**
         * @param name the name of the child dataset
         * @returns the high level type according to HLType and indicates group, dataset or named datatype. Useful when working with h5 in general and programmatically would like to know which part of the api to use
         */
        public getDatasetType(name: string): HLType;

        /**
         * 
         * @param name the name of the child dataset
         * @returns an array of length dataset rank and entries for the length of each dimension
         */
        public getDatasetDimensions(name: string): number[];

        /**
         * 
         * @param name the name of the child
         * @returns the type according to H5Type. Useful when working with h5 in general and programmatically would like to prepare for particular data type
         */
        public getDataType(name: string): H5Type;

        /**
         * 
         * @param name the name of the child dataset
         * @returns object with attributes
         */
        public getDatasetAttributes(name: string): object;

        /**
         * 
         * @param name 
         * @returns a Filters object for accessing the dataset’s filters
         */
        public getFilters(name: string): Filters;

        /**
         * 
         * @param idx unused starting over index
         * @param callback synchronous callback function returning H5Type and name
         */
        // file.iterate is not a function
        // public iterate(idx: number, callback: (type: H5Type, name: string) => void): void;

        /**
         * 
         * @param idx unused starting over index
         * @param callback synchronous callback function returning H5Type and xpath
         */
        // file.visit is not a function
        // public visit(idx: number, callback: (type: H5Type, xpath: string) => void): void;
    }

    class File extends Methods {

        constructor(filename: string, access: Access);

        /**
         * Turn on singe write, multi read mode
         */
        public enableSingleWriteMumltiRead(): void;

        /**
         * 
         * @param xpath the path to the group to create. Delimited with / and if any group along the stem doesn’t exist creates it too
         * @returns the new group object
         */
        public createGroup(xpath: string): Group;

        /**
         * 
         * @param name the name of the child dataset
         */
        public getByteOrder(name: string): H5TOrder;

    }

    class Group extends Methods {

        /**
         * 
         * @param xpath the path to the group to create. Delimited with / and if any group along the stem doesn’t exist creates it too
         * @returns the new group object
         */
        public create(xpath: string): Group;

        /**
         * 
         * @param xpath  the path to the group to open. Delimited with / and will throw an exception if it doesn’t exist. Useful particularly in read only mode
         * @returns the opened group object
         */
        public open(xpath: string): Group;

        /**
         * 
         * @param source the name of the source group
         * @param id the group or file id location. Every group and the file return with an id attribute. In this case it is the destination’s future location
         * @param destination the name of the destination group
         */
        public copy(source: string, id: Hdf5Id, destination: string): void;

        /**
         * 
         * @param source the name of the source group
         * @param id the group or file id location.Every group and the file return with an id attribute.In this case it is the destination’s future location
         * @param destination the name of the destination group
         */
        public link(source: string, id: Hdf5Id, destination: string): void;

    }

    // TODO
    class Filters {

    }

    function getLibVersion(): string;

}

export declare namespace h5lt {

    interface MakeDatasetOptions {

        /**
         * 0 and 9 inclusive. 0 is the lowest compression, but is the fastest. 9 gives the best compression, but is the slowest
         */
        compression?: number;
        fixed_width?: number;
        chunkSize?: number;

        maxRows?: number | H5SType;
        maxColumns?: number | H5SType;
        maxSections?: number | H5SType;
        maxFiles?: number | H5SType;
    }

    interface DatasetOptions {
        start: number[];

        /**
         * The stride to step over in each dimension
         */
        stride: number[];
        count: number[];
    }

    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @param buffer Typed array, string or a node::Buffer. The buffer can have attributes describing its shape. When using a node::Buffer the data is expected to be homogeneous and the type attribute set to choice in H5Type
     */
    function makeDataset(id: Hdf5Id, name: string, buffer: TypedArray | string | Hdf5Buffer): void;
    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @param buffer Typed array, string or a node::Buffer. The buffer can have attributes describing its shape. When using a node::Buffer the data is expected to be homogeneous and the type attribute set to choice in H5Type
     * @param options currently has the option to set compression { compression: 6}. can be 0 through 9. For an Array of fixed width strings {fixed_width: maxLength} option sets the width. The Array can hold variable length strings and this call will make a fixed buffer and if any are actually greater than maxLength an exception is thrown.
     */
    function makeDataset(id: Hdf5Id, name: string, buffer: TypedArray | string | Hdf5Buffer, options: MakeDatasetOptions): void;

    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @param buffer Typed array, string or a node::Buffer. The buffer can have attributes describing its shape. When using a node::Buffer the data is expected to be homogeneous and the type attribute set to choice in H5Type
     */
    function writeDataset(id: Hdf5Id, name: string, buffer: TypedArray | string | Hdf5Buffer): void;
    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @param buffer Typed array, string or a node::Buffer. The buffer can have attributes describing its shape. When using a node::Buffer the data is expected to be homogeneous and the type attribute set to choice in H5Type
     * @param options optional: start, stride and count
     */
    function writeDataset(id: Hdf5Id, name: string, buffer: string | Hdf5Buffer, options: DatasetOptions): void;

    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @returns A typed array or string
     */
    function readDataset(id: Hdf5Id, name: string): TypedArray | string;
    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset
     * @param options start, stride and count
     * @returns A typed array or string
     */
    function readDataset(id: Hdf5Id, name: string, options: DatasetOptions): TypedArray | string;

    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset.
     * @return A node::Buffer with the data and attributes for its shape and type
     */
    function readDatasetAsBuffer(id: Hdf5Id, name: string): Hdf5Buffer;
    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name a string naming the dataset.
     * @param options start, stride and count
     * @return A node::Buffer with the data and attributes for its shape and type
     */
    function readDatasetAsBuffer(id: Hdf5Id, name: string, options: DatasetOptions): Hdf5Buffer;

    // TODO
    function readDatasetDatatype(): any;
    function readDatasetLength(): any;
}

// TODO
export interface h5ds {
    attachScale: any;
    detachScale: any;
    getLabel: any;
    getNumberOfScales: any;
    getScaleName: any;
    isAttached: any;
    isScale: any;
    iterateScale: any;
    setLabel: any;
    setScale: any;
}

// TODO
export interface h5im {
    getImageInfo: any;
    isImage: any;
    makeImage: any;
    makePalette: any;
    readImage: any;
    readImageRegion: any;
}

// TODO
export interface h5pt {
    Int64: any;
    PacketTable: any;
    makeTable: any;
    readTable: any;
}

export declare namespace h5tb {

    /**
     * 
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name The table name
     * @param model a model composed of an Array of typed arrays and Array of strings each representing a column in the table. Each column has a name attribute to name it.
     */
    function makeTable(id: Hdf5Id, name: string, model: TableArray[]): void;

    /**
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name The table name
     * @param model a model composed of an Array of typed arrays and Array of strings each representing a column in the table. Each column has a name attribute to name it.
     */
    function appendRecords(id: Hdf5Id, name: string, model: TableArray[]): void;

    /**
     * @param id the group or file id location. Every group and the file return with an id attribute
     * @param name The table name
     * @param start &rarr starting record index
     * @param model a model composed of an Array of typed arrays and Array of strings each representing a column in the table. Each column has a name attribute to name it. . This is a sub model where the length of columns and start are less than the whole table
    */
    function writeRecords(id: Hdf5Id, name: string, start: number, model: TableArray[]): void;

    // TODO
    function addRecordsFrom(): any;
    function combineTables(): any;
    function deleteField(): any;
    function deleteRecord(): any;
    function getFieldInfo(): any;
    function getTableInfo(): any;
    function insertField(): any;
    function insertRecord(): any;
    function readFieldsIndex(): any;
    function readFieldsName(): any;
    function readRecords(): any;
    function readTable(): any;
    function writeFieldsIndex(): any;
    function writeFieldsName(): any;
}
