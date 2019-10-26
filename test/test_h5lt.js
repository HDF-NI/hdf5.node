'use strict';

(require('mocha'));
require("should");

const fs            = require('fs');
const parseString   = require('xml2js').parseString;
const util          = require('util');
const hdf5Lib       = require('..');
const globs         = require('../lib/globals');

const hdf5          = hdf5Lib.hdf5;
const h5lt          = hdf5Lib.h5lt;

const Access        = globs.Access;
const CreationOrder = globs.CreationOrder;
const State         = globs.State;
const HLType        = globs.HLType;
const H5Type        = globs.H5Type;
const H5OType       = globs.H5OType;

describe("testing lite interface ", function() {

    describe("create an h5, group and some datasets ", function() {
            this.timeout(10000);
        // open hdf file
        let file;
        before(function(done) {
          file = new hdf5.File('./h5lt.h5', Access.ACC_TRUNC);
          done();
        });
        let group;
        it("should be >0 ", function(done) {
            group=file.createGroup('pmc');
            group.id.should.not.equal(-1);
            done();
        });
        it("should be variable array of Uint8Array's", function(done) {
            const buffer=new Array(3);
            for(var i=0;i<3;i++){
                buffer[i]=new Uint8Array(i+1);
                for(var j=0;j<i+1;j++)
                    buffer[i][j]=j;
            }
            h5lt.makeDataset(group.id, 'real labels', buffer, {type: H5Type.H5T_STD_U8LE});
            const readBuffer=h5lt.readDataset(group.id, 'real labels', function(options){
                options.rank.should.equal(1);
                options.rows.should.equal(3);
            });
            readBuffer.constructor.name.should.match('Array');
            readBuffer.length.should.match(3);
            //readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.should.match(readBuffer);
            for(var i in readBuffer){
                console.log(readBuffer[i]);
            }
            //const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            //readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });
        it("should be Float64Array io ", function(done) {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Refractive Index', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index', function(options){
                options.rank.should.equal(1);
                options.rows.should.equal(5);
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });
        it("should be node::Buffer io for double le", function(done) {
            const buffer=Buffer.alloc(5*8, "\0", "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(4.0, 24);
            buffer.writeDoubleLE(5.0, 32);
            h5lt.makeDataset(group.id, 'Dielectric Constant', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Dielectric Constant', function(options){
                options.rank.should.equal(1);
                options.rows.should.equal(5);
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Dielectric Constant');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });
        it("should be node::Buffer io for double rank data", function(done) {
            const buffer=Buffer.alloc(6*8, "\0", "binary");
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(1.0, 24);
            buffer.writeDoubleLE(2.0, 32);
            buffer.writeDoubleLE(3.0, 40);
            h5lt.makeDataset(group.id, 'Two Rank', buffer, {type: H5Type.H5T_NATIVE_DOUBLE, rank: 2, rows: 3, columns: 2});
            var byteOrder=group.getByteOrder('Two Rank');
            byteOrder.should.equal(0);
            const readBuffer=h5lt.readDataset(group.id, 'Two Rank', function(options) {
                    JSON.stringify(options).should.equal('{"rank":2,"endian":0,"rows":3,"columns":2}')
                });
                // console.dir(" after options cb: ");
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(6);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Two Rank');
            readAsBuffer.readDoubleLE(4*8).should.equal(2.0);
            readAsBuffer.rows.should.match(3);
            readAsBuffer.columns.should.match(2);
            done();
        });
        it("should be node::Buffer io for quadruple rank data", function(done) {
            const buffer=Buffer.alloc(2*2*6*8, "\0", "binary");
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            buffer.writeDoubleLE(1.0, 0);
            buffer.writeDoubleLE(2.0, 8);
            buffer.writeDoubleLE(3.0, 16);
            buffer.writeDoubleLE(1.0, 24);
            buffer.writeDoubleLE(2.0, 32);
            buffer.writeDoubleLE(3.0, 40); //6
            buffer.writeDoubleLE(2.0, 48);
            buffer.writeDoubleLE(4.0, 56);
            buffer.writeDoubleLE(6.0, 64);
            buffer.writeDoubleLE(2.0, 72);
            buffer.writeDoubleLE(4.0, 80);
            buffer.writeDoubleLE(6.0, 88); //12
            buffer.writeDoubleLE(3.0, 96);
            buffer.writeDoubleLE(6.0, 104);
            buffer.writeDoubleLE(9.0, 112);
            buffer.writeDoubleLE(3.0, 120);
            buffer.writeDoubleLE(6.0, 128);
            buffer.writeDoubleLE(9.0, 136); //18
            buffer.writeDoubleLE(4.0, 144);
            buffer.writeDoubleLE(8.0, 152);
            buffer.writeDoubleLE(12.0, 160);
            buffer.writeDoubleLE(4.0, 168);
            buffer.writeDoubleLE(8.0, 176);
            buffer.writeDoubleLE(12.0, 184); //24
            buffer.rank=4;
            buffer.rows=3;
            buffer.columns=2;
            buffer.sections=2;
            buffer.files=2;
            h5lt.makeDataset(group.id, 'Quadruple Rank', buffer);
            var byteOrder=group.getByteOrder('Quadruple Rank');
            byteOrder.should.equal(0);
            const readBuffer=h5lt.readDataset(group.id, 'Quadruple Rank', function(options) {
                    JSON.stringify(options).should.equal('{"rank":4,"endian":0,"rows":3,"columns":2,"sections":2,"files":2}')
                });
                // console.dir(" after options cb: ");
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(24);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Quadruple Rank');
            readAsBuffer.readDoubleLE(4*8).should.equal(2.0);
            readAsBuffer.rows.should.match(3);
            readAsBuffer.columns.should.match(2);
            readAsBuffer.sections.should.match(2);
            readAsBuffer.files.should.match(2);
            done();
        });
        it("should be Float32Array io ", function(done) {
            const buffer=new Float32Array(5);
            buffer[0]=5.0;
            buffer[1]=4.0;
            buffer[2]=3.0;
            buffer[3]=2.0;
            buffer[4]=1.0;
            h5lt.makeDataset(group.id, 'Refractive Index f', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index f');
            readBuffer.constructor.name.should.match('Float32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Int32Array io ", function(done) {
            const buffer=new Int32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index l', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index l');
            readBuffer.constructor.name.should.match('Int32Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Uint32Array io ", function(done) {
            const buffer=new Uint32Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index ui', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index ui');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint32Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Int16Array io ", function(done) {
            const buffer=new Int16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index s', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index s');
            readBuffer.constructor.name.should.match('Int16Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Uint16Array io ", function(done) {
            const buffer=new Uint16Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index us', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index us');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint16Array');
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Int8Array io ", function(done) {
            const buffer=new Int8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            h5lt.makeDataset(group.id, 'Refractive Index 8', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index 8');
            readBuffer.constructor.name.should.match('Int8Array');
            readBuffer.length.should.match(5);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            done();
        });
        it("should be Uint8Array io ", function(done) {
            const buffer=new Uint8Array(5);
            buffer[0]=5;
            buffer[1]=4;
            buffer[2]=3;
            buffer[3]=2;
            buffer[4]=1;
            buffer.rank=1;
            buffer.rows=5;
            buffer.type=H5Type.H5T_NATIVE_UCHAR;
            h5lt.makeDataset(group.id, 'Refractive Index u8', buffer);
            const readBuffer=h5lt.readDataset(group.id, 'Refractive Index u8');
            readBuffer.length.should.match(5);
            readBuffer.constructor.name.should.match('Uint8Array');
            buffer.rank=1;
            buffer.rows=5;
            //buffer.should.match(readBuffer);
            done();
        });

        it("should be make a dataset with compression ", function(done) {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Compressed Index', buffer, { compression: 7});
            const readBuffer=h5lt.readDataset(group.id, 'Compressed Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });

        it("should be make a dataset with custom chunk size ", function(done) {
            const buffer=new Float64Array(5);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            h5lt.makeDataset(group.id, 'Custom-chunked Index', buffer, { chunkSize: 2});
            const readBuffer=h5lt.readDataset(group.id, 'Custom-chunked Index');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(5);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.rank=1;
            buffer.rows=5;
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Refractive Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });

        it("should be make a dataset with arrayed chunk size ", function(done) {
            const buffer=new Float64Array(10);
            buffer[0]=1.0;
            buffer[1]=2.0;
            buffer[2]=3.0;
            buffer[3]=4.0;
            buffer[4]=5.0;
            buffer[5]=1.0;
            buffer[6]=2.0;
            buffer[7]=3.0;
            buffer[8]=4.0;
            buffer[9]=5.0;
            h5lt.makeDataset(group.id, 'Arrayed-chunked Index', buffer, {rank:2, rows: 5, columns:2, chunkSize: [3,2]});
            const readBuffer=h5lt.readDataset(group.id, 'Arrayed-chunked Index', function(options){
                
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(10);
            readBuffer.buffer.byteLength.should.match(buffer.buffer.byteLength);
            buffer.should.match(readBuffer);
            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Arrayed-chunked Index');
            readAsBuffer.readDoubleLE(4*8).should.equal(5.0);
            done();
        });

        it("flush properties to h5 ", function(done){
            group.getNumAttrs().should.equal(0);
            group[ 'Computed Heat of Formation' ]=100.0;
            group.flush();
            group.getNumAttrs().should.equal(1);
            group[ 'Computed Heat of Formation' ]=7.77;
            group.flush();
            group.getNumAttrs().should.equal(1);
            group.Status=256;
            group.flush();
            group.getNumAttrs().should.equal(2);
            group.Status=-1;
            group.flush();
            group.getNumAttrs().should.equal(2);
            group.Information="\"There are no solutions; there are only trade-offs.\" -- Thomas Sowell";
            group.flush();
            group.getNumAttrs().should.equal(3);
            done();
        });
        it("should close pmc ", function(done){
            try{
            group.close();
        }
        catch(err){
            console.log("err "+err.message);
        }
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });

    describe("create an h5, group and some documents ", function() {
            this.timeout(35000);
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_TRUNC);
          done();
        });

        it("open of Geometries should be >0", function(done) {
            const groupPMCServices=file.createGroup('pmcservices');
            const groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
            groupTargets[ 'Computed Heat of Formation' ]=-221.78436098572274;
            groupTargets[ 'Computed Ionization Potential' ]=9.57689311885752;
            groupTargets[ 'Computed Total Energy' ]=-3573.674399276322;
            groupTargets.flush();
            const groupDocuments      = file.createGroup('pmcservices/sodium-icosanoate/Documents');
            const groupFrequencyData  = file.createGroup('pmcservices/sodium-icosanoate/Frequency Data');
            const groupTrajectories   = file.createGroup('pmcservices/sodium-icosanoate/Trajectories');
            const sodiumIcosanoateXml = fs.readFileSync("./test/examples/sodium-icosanoate.xml", "ascii");
            h5lt.makeDataset(groupDocuments.id, 'sodium-icosanoate.xml', sodiumIcosanoateXml);
            groupTrajectories.close();
            groupFrequencyData.close();
            groupDocuments.close();

            const sodiumIcosanoateXmol = fs.readFileSync("./test/examples/sodium-icosanoate.xmol", "ascii");
            let count              = 0;
            let numberOfDataLines;
            let title;
            let state              = State.COUNT;
            const lineArr          =  sodiumIcosanoateXmol.trim().split("\n");
            let columnCount        = 0;
            let firstFrequency     = true;
            let firstTrajectory    = new Float64Array(3*numberOfDataLines);
            let lastTrajectory     = new Float64Array(3*numberOfDataLines);
            let frequency          = new Float64Array(3*numberOfDataLines);
            /* Loop over every line. */
            lineArr.forEach(function (line) {
                switch(state) {
                    case State.COUNT:
                        numberOfDataLines = parseInt(line);
                        firstTrajectory   = new Float64Array(3*numberOfDataLines);
                        lastTrajectory    = new Float64Array(3*numberOfDataLines);
                        frequency         = new Float64Array(3*numberOfDataLines);
                        state             = State.TITLE;
                        break;
                    case State.TITLE:
                        title=line;
                        state=State.DATA;
                        break;
                    case State.DATA:
                        const columnArr = line.split(" ");
                        columnArr.forEach(function (value) {
                        switch(columnCount)
                        {
                            case 0:
                                break;
                            case 1:
                            case 2:
                            case 3:
                                firstTrajectory[3*count+columnCount-1]=parseFloat(value);
                                lastTrajectory[3*count+columnCount-1]=parseFloat(value);
                                break;
                            case 4:
                            case 5:
                            case 6:
                                frequency[3*count+columnCount-4]=parseFloat(value);
                                if(columnCount===6){ count++; }
                                break;
                        }
                        columnCount++;
                        if(columnCount===7){columnCount=0;}
                        if(count === numberOfDataLines){
                        count=0;
                        if(firstFrequency)
                        {
                            try{
                            const groupGeometries=file.createGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
                            firstTrajectory.Dipole=2.9;
                            h5lt.makeDataset(groupGeometries.id, '0', firstTrajectory, {rank: 2, rows: numberOfDataLines, columns: 3});
                            h5lt.makeDataset(groupGeometries.id, '1', lastTrajectory, {rank: 2, rows: numberOfDataLines, columns: 3});
                            const groupFrequencies=file.createGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
                            groupFrequencies.close();
                            groupGeometries.close();
                            firstFrequency=false;
                            }
                            catch(err){
                                console.dir("what? "+err.message);
                            }
                        }
                            const groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
                            h5lt.makeDataset(groupFrequencies.id, title, frequency, {rank: 2, rows: numberOfDataLines, columns: 3});
                            state=State.COUNT;
                            groupFrequencies.close();
                        }
                        });
                        break;
                }
            });
            groupTargets.close();
            groupPMCServices.close();
            done();
        });

        it("Existing group should throw exception when trying to create again ", function(done){
            try {
                const groupTargets=file.createGroup('pmcservices/sodium-icosanoate');
                groupTargets.close();
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            try {
                const groupDocuments=file.createGroup('pmcservices/sodium-icosanoate/Documents');
                groupDocuments.close();
            } catch(err) {
                err.message.should.equal("");
                console.dir(err.message);
            }
            done();
        });

        after(function(done) {
          file.close();
          done();
        });
    });

    describe("create dataset and extract subset", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
          done();
        });

        it("should be node::Buffer io for double rank hyperslab", function(done) {
            const group=file.createGroup('pmcservices/Double');
            const buffer=Buffer.alloc(8*10*8);
            for (let j = 0; j < 10; j++) {
                for (let i = 0; i < 8; i++){
                        if (j< (10/2)) {
                            buffer.writeDoubleLE(1.0, 8*(i*10+j));
                        } else {
                            buffer.writeDoubleLE(2.0, 8*(i*10+j));
                        }
                }
            }

            h5lt.makeDataset(group.id, 'Waldo', buffer, {type: H5Type.H5T_NATIVE_DOUBLE, rank: 2, rows: 8, columns: 10});
            var dimensions=group.getDatasetDimensions('Waldo');
            dimensions.length.should.equal(2);
            dimensions[0].should.equal(8);
            dimensions[1].should.equal(10);
            const subsetBuffer=Buffer.alloc(3*4*8, "\0", "binary");
            subsetBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let j = 0; j < 4; j++) {
                for (let i = 0; i < 3; i++){
                            subsetBuffer.writeDoubleLE(5.0, 8*(i*4+j));
                }
            }

            h5lt.writeDataset(group.id, 'Waldo', subsetBuffer, {start: [1,2], stride: [1,1], count: [3,4]});
            let theType=group.getDataType('Waldo');
            theType.should.equal(H5Type.H5T_IEEE_F64LE);
            const readBuffer=h5lt.readDataset(group.id, 'Waldo', function(options) {
                options.rank.should.equal(2);
                options.rows.should.match(8);
                options.columns.should.equal(10);
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(8*10);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Waldo', {start: [3,4], stride: [1,1], count: [2,2]});
            readAsBuffer.rank.should.equal(2);
            readAsBuffer.rows.should.equal(2);
            readAsBuffer.columns.should.equal(2);
            readAsBuffer.readDoubleLE(0*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(1*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(2*8).should.equal(1.0);
            readAsBuffer.readDoubleLE(3*8).should.equal(2.0);
            readAsBuffer.type.should.equal(H5Type.H5T_IEEE_F64LE);
            group.close();
            done();
        });
        it("should be node::Buffer io for triple rank hyperslab", function(done) {
            const group=file.createGroup('pmcservices/Triple');
            const buffer=Buffer.alloc(3*8*10*8, "\0", "binary");
            buffer.rank=3;
            buffer.rows=8;
            buffer.columns=10;
            buffer.sections=3;
            buffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let k = 0; k < buffer.sections; k++) {
                for (let j = 0; j < buffer.columns; j++) {
                    for (let i = 0; i < buffer.rows; i++){
                            if (j< (buffer.columns/2)) {
                                buffer.writeDoubleLE(1.0, 8*(k*buffer.columns*buffer.rows+i*buffer.columns+j));
                            } else {
                                buffer.writeDoubleLE(2.0, 8*(k*buffer.columns*buffer.rows+i*buffer.columns+j));
                            }
                    }
                }
            }
            h5lt.makeDataset(group.id, 'Waldo', buffer);
            var dimensions=group.getDatasetDimensions('Waldo');
            dimensions.length.should.equal(3);
            dimensions[0].should.equal(3);
            dimensions[1].should.equal(8);
            dimensions[2].should.equal(10);
            const subsetBuffer=Buffer.alloc(3*4*8, "\0", "binary");
            subsetBuffer.rank=3;
            subsetBuffer.rows=3;
            subsetBuffer.columns=4;
            subsetBuffer.sections=1;
            subsetBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let k = 0; k < subsetBuffer.sections; k++) {
                for (let j = 0; j < subsetBuffer.columns; j++) {
                    for (let i = 0; i < subsetBuffer.rows; i++){
                                subsetBuffer.writeDoubleLE(5.0, 8*(k*buffer.columns*buffer.rows+i*subsetBuffer.columns+j));
                    }
                }
            }
//
            h5lt.writeDataset(group.id, 'Waldo', subsetBuffer, {start: [1,2,1], stride: [1,1,1], count: [1,3,4]});
            let theType=group.getDataType('Waldo');
            theType.should.equal(H5Type.H5T_IEEE_F64LE);
            const readBuffer=h5lt.readDataset(group.id, 'Waldo');
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(3*8*10);
            readBuffer.rows.should.match(8);
            readBuffer.columns.should.match(10);
            readBuffer.sections.should.match(3);

            const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Waldo', {start: [1,3,4], stride: [1,1,1], count: [1,2,2]});
            readAsBuffer.readDoubleLE(0*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(1*8).should.equal(2.0);
            readAsBuffer.readDoubleLE(2*8).should.equal(5.0);
            readAsBuffer.readDoubleLE(3*8).should.equal(2.0);
            readAsBuffer.type.should.equal(H5Type.H5T_IEEE_F64LE);
            group.close();
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });

    describe.skip("should read loom attributes", function() {
        let file;
        before(function(done) {
          file = new hdf5Lib.hdf5.File('/home/roger/Downloads/hgForebrainGlut.loom', globs.Access.ACC_RDONLY);
          done();
        });

        it("should be slab info ", function(done) {
            console.log(file);
            console.log(file.getNumAttrs());
            file.refresh();
            for (var property in file) {
                if (file.hasOwnProperty(property)) {
                    console.log(property+": "+file[property]);
                }
            }
            var dim = file.getDatasetDimensions('matrix');
            console.log(dim.length);
            console.log(dim);
            for (var i = 0; i < dim[0]/1000; i++) {
              var buffer = h5lt.readDatasetAsBuffer(file.id, 'matrix', {
                  start: [i, 0],
                  stride: [1, 1],
                  count: [1, dim[1]]
                });
            }
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

    describe("create dataset and fail extracting subset", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
          done();
        });

        it("should fail when count is not given for the subset", function(done) {
            const group=file.createGroup('pmcservices/Forest');
            const buffer=Buffer.alloc(8*10*8);
            for (let j = 0; j < 10; j++) {
                for (let i = 0; i < 8; i++){
                        if (j< (10/2)) {
                            buffer.writeDoubleLE(1.0, 8*(i*10+j));
                        } else {
                            buffer.writeDoubleLE(2.0, 8*(i*10+j));
                        }
                }
            }
            h5lt.makeDataset(group.id, 'Waldo', buffer, {type: H5Type.H5T_NATIVE_DOUBLE, rank: 2, rows: 8, columns: 10});
            var dimensions=group.getDatasetDimensions('Waldo');
            dimensions.length.should.equal(2);
            dimensions[0].should.equal(8);
            dimensions[1].should.equal(10);
            const subsetBuffer=Buffer.alloc(3*4*8, "\0", "binary");
            subsetBuffer.type=H5Type.H5T_NATIVE_DOUBLE;
            for (let j = 0; j < 4; j++) {
                for (let i = 0; i < 3; i++){
                            subsetBuffer.writeDoubleLE(5.0, 8*(i*4+j));
                }
            }

            h5lt.writeDataset(group.id, 'Waldo', subsetBuffer, {start: [1,2], stride: [1,1], count: [3,4]});
            let theType=group.getDataType('Waldo');
            theType.should.equal(H5Type.H5T_IEEE_F64LE);
            const readBuffer=h5lt.readDataset(group.id, 'Waldo', function(options) {
                options.rank.should.equal(2);
                options.rows.should.match(8);
                options.columns.should.equal(10);
            });
            readBuffer.constructor.name.should.match('Float64Array');
            readBuffer.length.should.match(8*10);

            try{
              const readAsBuffer=h5lt.readDatasetAsBuffer(group.id, 'Waldo', {start: [3,4], stride: [1,1]});
            }
            catch (e) {
                e.message.should.equal("Need to supply the subspace count dimensions. Start and stride are optional.");
            }
            group.close();
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });
    
    describe("varlen char arrays", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
          done();
        });
        it("create array of varlen's", function(done) {
            let group=file.createGroup('pmcservices/Quotes');
            const quotes=new Array(7);
            quotes[0]="Never put off till tomorrow what may be done day after tomorrow just as well.";
            quotes[1]="I have never let my schooling interfere with my education";
            quotes[2]="Reader, suppose you were an idiot. And suppose you were a member of Congress. But I repeat myself.";
            quotes[3]="Substitute 'damn' every time you're inclined to write 'very;' your editor will delete it and the writing will be just as it should be.";
            quotes[4]="Don’t go around saying the world owes you a living. The world owes you nothing. It was here first.";
            quotes[5]="Loyalty to country ALWAYS. Loyalty to government, when it deserves it.";
            quotes[6]="What would men be without women? Scarce, sir...mighty scarce.";
            h5lt.makeDataset(group.id, "Mark Twain", quotes);
            group.close();
            file.close();
            file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
            group=file.openGroup('pmcservices/Quotes');
            const array=h5lt.readDataset(group.id, 'Mark Twain');
            group.close();
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });

    describe("fixed char multi-dimension arrays", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
          done();
        });
        it("create 2d array of strings", function(done) {
           try{
           let group=file.createGroup('pmcservices');
            const rotation=new Array(3);
            rotation[0]=new Array(3);
            rotation[0][0]="1";
            rotation[0][1]="0";
            rotation[0][2]="0";
            rotation[1]=new Array(3);
            rotation[1][0]="0";
            rotation[1][1]="\\cos\\theta";
            rotation[1][2]="-\\sin\\theta";
            rotation[2]=new Array(3);
            rotation[2][0]="0";
            rotation[2][1]="\\sin\\theta";
            rotation[2][2]="\\cos\\theta";



            h5lt.makeDataset(group.id, "RotationX", rotation, {fixed_width : 12});
            group.close();
            file.close();
            file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
            group=file.openGroup('pmcservices');
            const matrix=h5lt.readDataset(group.id, 'RotationX');
            matrix.length.should.equal(3);
            matrix[1].length.should.equal(3);
            matrix[1][1].should.equal("\\cos\\theta");
            console.dir(matrix);
            group.close();
            } catch (e) {
                console.log(e.message);
            }
            done();
        });
        it("create 2d array of strings with padding", function(done) {
            let group;
           try{
            group=file.openGroup('pmcservices');
            const rotation=new Array(3);
            rotation[0]=new Array(3);
            rotation[0][0]="\\cos\\theta";
            rotation[0][1]="-\\sin\\theta";
            rotation[0][2]="0";
            rotation[1]=new Array(3);
            rotation[1][0]="\\sin\\theta";
            rotation[1][1]="\\cos\\theta";
            rotation[1][2]="0";
            rotation[2]=new Array(3);
            rotation[2][0]="0";
            rotation[2][1]="0";
            rotation[2][2]="1";



            h5lt.makeDataset(group.id, "RotationZ", rotation, {fixed_width : 12, padding : H5Type.H5T_STR_SPACEPAD});
            group.close();
            file.close();
            file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
            group=file.openGroup('pmcservices');
            const matrix=h5lt.readDataset(group.id, 'RotationZ');
            matrix.length.should.equal(3);
            matrix[1].length.should.equal(3);
            matrix[1][1].should.equal("\\cos\\theta");
            console.dir(matrix);
            group.close();
            } catch (e) {
                console.log("bad "+e.message);
                group.close();
            }
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });

    describe("huge varlen char arrays", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
          done();
        });
        it("create huge array of varlen's", function(done) {
            this.timeout(7000);
            let group=file.createGroup('pmcservices/Huge Quotes');
            const quotes=new Array(7);
            quotes[0]="Never put off till tomorrow what may be done day after tomorrow just as well.";
            quotes[1]="I have never let my schooling interfere with my education";
            quotes[2]="Reader, suppose you were an idiot. And suppose you were a member of Congress. But I repeat myself.";
            quotes[3]="Substitute 'damn' every time you're inclined to write 'very;' your editor will delete it and the writing will be just as it should be.";
            quotes[4]="Don’t go around saying the world owes you a living. The world owes you nothing. It was here first.";
            quotes[5]="Loyalty to country ALWAYS. Loyalty to government, when it deserves it.";
            quotes[6]="What would men be without women? Scarce, sir...mighty scarce.";

            const hugeQuotes=new Array(250000);
            for(var i =0;i<250000;i++){
                hugeQuotes[i]=quotes[i % 7];
            }
            h5lt.makeDataset(group.id, "Mark Twain", hugeQuotes);
            group.close();
            file.close();
            file = new hdf5.File('./pmc.h5', Access.ACC_RDWR);
            group=file.openGroup('pmcservices/Huge Quotes');
            const array=h5lt.readDataset(group.id, 'Mark Twain', {start: [1000], stride: [1], count: [21]});
            console.dir(array[0]+" -- Mark Twain");
            array.length.should.equal(21);
            group.close();
            done();
        });

        after(function(done) {
          file.close();
          done();
        });
    });
    
    describe("varlen chars", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./test/examples/nba.h5', Access.ACC_RDONLY);
          done();
        });
        it("read varlen's", function(done) {
            const array=h5lt.readDataset(file.id, 'player');
            array.length.should.equal(500);
            if(array.constructor.name==='Array'){
                for(let mIndex=0;mIndex<array.length;mIndex++){
                    //console.dir(array[mIndex]);
                }
            }
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });


    describe("write/read enum", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./enum.h5', Access.ACC_TRUNC);
          done();
        });
        it("write enum", function(done) {
            var phases = {
                SOLID: 0,
                LIQUID: 1,
                GAS: 2,
                PLASMA: 3
            };
            const buffer=new Uint16Array(35);
            buffer[0]=phases.SOLID,  buffer[2]=phases.SOLID,  buffer[3]=phases.SOLID,  buffer[4]=phases.SOLID,  buffer[5]=phases.SOLID,  buffer[6]=phases.SOLID,  buffer[7]=phases.SOLID;
            buffer[8]=phases.SOLID,  buffer[9]=phases.LIQUID, buffer[10]=phases.GAS,    buffer[11]=phases.PLASMA, buffer[12]=phases.SOLID,  buffer[13]=phases.LIQUID, buffer[14]=phases.GAS;
            buffer[15]=phases.SOLID,  buffer[16]=phases.GAS,    buffer[17]=phases.SOLID,  buffer[18]=phases.GAS,    buffer[19]=phases.SOLID,  buffer[20]=phases.GAS,    buffer[21]=phases.SOLID;
            buffer[22]=phases.SOLID,  buffer[23]=phases.PLASMA, buffer[24]=phases.GAS,    buffer[2]=phases.LIQUID, buffer[26]=phases.SOLID,  buffer[27]=phases.PLASMA, buffer[28]=phases.GAS;
            h5lt.makeDataset(file.id, 'states', buffer, {rank: 2, rows: 5, columns: 7, enumeration: phases});

            done();
        });
        it("read enum", function(done) {
            const enumeration=h5lt.readDataset(file.id, 'states', (options) =>{
                console.dir(options.enumeration);
            });
            enumeration.length.should.equal(35);
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });

    describe.skip("reading inchies", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('/home/roger/Downloads/inchies/inchies_2.h5', Access.ACC_RDWR);
          done();
        });
        it("read an inchie data", function(done) {
var start = process.hrtime();
            const array=h5lt.readDataset(file.id, 'inchies');
    var elapsed = process.hrtime(start)[1] / 1000000; // divide by a million to get nano to milli
    console.log(process.hrtime(start)[0] + " s, " + elapsed.toFixed(4) + " ms"); // print message + time
            console.dir(array.length);
            console.dir(array[0]);
            array.length.should.equal(1000000);
            var ll=27;
            var hl=0;
            for(var i in array){
                if(array[i].length<ll)ll=array[i].length;
                if(array[i].length>hl)hl=array[i].length;
            }
            console.dir(ll+" "+hl);
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });
    
    describe("create an xmol with frequency pulled from h5 ", function() {
            this.timeout(35000);
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDONLY);
          done();
        });
        let groupTarget;
        it("open of target should be >0", function(done) {
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
            done();
        });
        it("getNumAttrs of groupTarget should be 3", function(done) {
            groupTarget.getNumAttrs().should.equal(3);
            groupTarget.refresh();

            it("readAttribute Computed Heat Of Formation should be -221.78436098572274", function(done) {
                groupTarget.refresh();
                groupTarget[ 'Computed Heat Of Formation' ].should.equal(-221.78436098572274);
                groupTarget[ 'Computed Ionization Potential' ].should.equal(9.57689311885752);
                groupTarget[ 'Computed Total Energy' ].should.equal(-3573.674399276322);
                done();
            });
            done();
        });
        it("open of Geometries should be >0", function(done){
            const groupDocuments = file.openGroup('pmcservices/sodium-icosanoate/Documents');
            const xmlDocument    = h5lt.readDataset(groupDocuments.id, 'sodium-icosanoate.xml');
            parseString(xmlDocument, function (err, result) {
            const molecule=result['cml:cml']['cml:molecule'][0];

            const elements     =[];
            let elementIndex =0;
            for (let moleculeIndex = 0; moleculeIndex < molecule['cml:molecule'].length; moleculeIndex++)
            {
                const atoms=molecule['cml:molecule'][moleculeIndex]['cml:atomArray'][0]['cml:atom'];
                elements.length+=atoms.length;
                for (let index = 0; index < atoms.length; index++)
                {
                    elements[elementIndex] = util.inspect(atoms[index].$.elementType, false, null);
                    elements[elementIndex] = elements[elementIndex].substr(1,elements[elementIndex].length -2);
                    elementIndex++;
                }
            }
            const groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            const array=groupGeometries.getMemberNamesByCreationOrder();
            const groupFrequencies=file.openGroup('pmcservices/sodium-icosanoate/Frequency Data/Frequencies');
            const frequencyNames=groupFrequencies.getMemberNamesByCreationOrder();

            array[1].should.equal("1");
            let xmolDocument="";
            groupGeometries.getDatasetType(array[1]).should.equal(HLType.HL_TYPE_LITE);
            const lastTrajectory=h5lt.readDataset(groupGeometries.id, array[1],  function(options) {
              options.rank.should.equal(2);
              options.columns.should.equal(3);
            });
                for (let frequencyIndex = 0; frequencyIndex < frequencyNames.length; frequencyIndex++)
                {
                    xmolDocument+=elements.length+'\n';
                    xmolDocument+=frequencyNames[frequencyIndex]+'\n';
                    groupFrequencies.getDatasetType(frequencyNames[frequencyIndex]).should.equal(HLType.HL_TYPE_LITE);
                    const frequency=h5lt.readDataset(groupFrequencies.id, frequencyNames[frequencyIndex]);
                    for (let index = 0; index < elements.length; index++)
                    {
                        xmolDocument+=elements[index]+' '+lastTrajectory[3*index]+' '+lastTrajectory[3*index+1]+' '+lastTrajectory[3*index+2]+' '+frequency[3*index]+' '+frequency[3*index+1]+' '+frequency[3*index+2]+'\n';
                    }
                }
                xmolDocument.length.should.equal(1435803);
                fs.writeFileSync('sodium-icosanoate.xmol', xmolDocument, { flag:'w'});
                fs.writeFileSync('sodium-icosanoate.xml', xmlDocument, { flag:'w'});
                groupGeometries.close();
                groupFrequencies.close();
                groupTarget.close();
            });
            groupDocuments.close();
            done();
        });

        let groupGeometries;
        it("open of Geometries should be >0", function(done){
            groupGeometries=file.openGroup('pmcservices/sodium-icosanoate/Trajectories/Geometries');
            groupGeometries.id.should.not.equal(-1);
            done();
        });
        it("getNumAttrs of Geometries should be 0", function(done){
            groupGeometries.getNumAttrs().should.equal(0);
            done();
        });
        it("getNumObjs of Geometries should be 2", function(done){
            groupGeometries.getNumObjs().should.equal(2);
            done();
        });
        it("getMemberNames of Geometries should be 240 names in creation order", function(done){
            const array=groupGeometries.getMemberNamesByCreationOrder();
            array[1].should.equal("1");
            done();
        });
        it("Size of dataset '0' should be 186 ", function(done) {
            groupGeometries.getDatasetType('0').should.equal(HLType.HL_TYPE_LITE);
            const readBuffer=h5lt.readDataset(groupGeometries.id, '0', {bind_attributes:true});
            'Float64Array'.should.match(readBuffer.constructor.name);
            const length=186;
            length.should.match(readBuffer.length);
            console.dir(readBuffer.Dipole);
            const value=2.9;
            value.should.match(readBuffer.Dipole);
            done();
        });
        it("Get dataset attributes", function(done) {
            groupGeometries.getDatasetType('0').should.equal(HLType.HL_TYPE_LITE);
            const readBuffer=h5lt.readDataset(groupGeometries.id, '0');
            'Float64Array'.should.match(readBuffer.constructor.name);
            const length=186;
            length.should.match(readBuffer.length);
            var attrs=groupGeometries.getDatasetAttributes('0');
            const value=2.9;
            value.should.match(attrs.Dipole);
            done();
        });
        it("Get dataset individual attribute", function(done) {
            groupGeometries.getDatasetType('0').should.equal(HLType.HL_TYPE_LITE);
            const readBuffer=h5lt.readDataset(groupGeometries.id, '0');
            'Float64Array'.should.match(readBuffer.constructor.name);
            const length=186;
            length.should.match(readBuffer.length);
            var attr=groupGeometries.getDatasetAttribute('0', 'Dipole');
            console.dir(attr);
            const value=2.9;
            value.should.match(attr);
            groupGeometries.close();
            done();
        });
        it("getNumAttrs of file should be 3", function(done) {
            file.getNumAttrs().should.equal(0);
            file.refresh();
            done();
        });
        after(function(done) {
          file.close();
          done();
        });
    });
    
    describe("iterations on h5 ", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./pmc.h5', Access.ACC_RDONLY);
          done();
        });
        let groupTarget;
        it.skip("iterate thru", function(done) {
            groupTarget=file.openGroup('pmcservices', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
            var paths=[];
            paths.push('sodium-icosanoate');
            paths.push('Triple');
            paths.push('Double');
            paths.push('Quotes');
            paths.push('namForest');
            try {
                var count=0;
                groupTarget.iterate(1, function(r, name) {
                    paths[count].should.equal(name);
                    count++;
                });
            } catch (e) {

            }
            groupTarget.close();
            done();
        });
        
        it.skip("visit thru", function(done) {
            groupTarget=file.openGroup('pmcservices/sodium-icosanoate/Documents', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.id.should.not.equal(-1);
            try {
                var count=0;
                file.visit(1, function(r, xpath) {
                    //console.dir("visiting name: "+xpath);
                    count++;
                });
                console.log("cout "+count);
                count.should.equal(204);
            } catch (e) {
                console.log(e.message);
            }
            groupTarget.close();
            done();
        });
        
        after(function(done) {
          file.close();
          done();
        });
    });

});
