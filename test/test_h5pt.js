'use strict';

(require('mocha'));
require('should');

const hdf5Lib       = require('..');
const globs         = require('../lib/globals');

const hdf5          = hdf5Lib.hdf5;
const h5pt          = hdf5Lib.h5pt;
const Access        = globs.Access;
const CreationOrder = globs.CreationOrder;
const HLType        = globs.HLType;

describe("testing table interface ", function() {

    describe("create an h5, group and some tables ",function() {
        // open hdf file
        let file;
        before(function(done) {
          file = new hdf5.File('./h5pt.h5', Access.ACC_TRUNC);
          done();
        });

        it("should be Table io ", function(done) {
            const group=file.createGroup('pmc/refinement');
            group.id.should.not.equal(-1);
            const table  = new h5pt.PacketTable(new hdf5.Int64(0), 5);
            table.record = {};
            table.record.Set          = "Single Point";
            table.record["Date Time"] = "Mon Nov 24 13:10:44 2014";
            table.record.Name         = "Temperature";
            table.record.Value        = "37.4";
            table.record.Units        = "Celcius";

            h5pt.makeTable(group.id, 'Events', table);
            table.record.Set          = "Single Point";
            table.record["Date Time"] = "Mon Nov 24 13:20:44 2014";
            table.record.Name         = "Temperature";
            table.record.Value        = "37.3";
            table.record.Units        = "Celcius";
            table.append();
            table.record.Set          = "Single Point";
            table.record["Date Time"] = "Mon Nov 24 13:20:44 2014";
            table.record.Name         = "Temperature";
            table.record.Value        = "37.5";
            table.record.Units        = "Celcius";
            table.append();
            table.close();
            group.close();
            done();
        });

        it("should close pmc/refinement", function(done) {
            file.close();
            done();
        });
    });

    describe("should read table", function() {
        let file;
        before(function(done) {
          file = new hdf5.File('./h5pt.h5', Access.ACC_RDONLY);
          done();
        });

        it("should be Table input ", function(done){
            const groupTarget=file.openGroup('pmc/refinement', CreationOrder.H5P_CRT_ORDER_TRACKED| CreationOrder.H5P_CRT_ORDER_TRACKED);
            groupTarget.getDatasetType("Events").should.equal(HLType.HL_TYPE_PACKET_TABLE);
            const table = h5pt.readTable(groupTarget.id, "Events");

            const actualKeys   = Object.keys(table.record);
            const expectedKeys = [ 'Set', 'Date Time', 'Name', 'Value', 'Units' ];
            for (let i = 0; i < actualKeys.length; ++i) {
                actualKeys[i].should.equal(expectedKeys[i]);
            }

            let idx = 0;
            const expectedVals = ['Single Point Mon Nov 24 13:10:44 2014 Temperature 37.4 Celcius',
                                  'Single Point Mon Nov 24 13:20:44 2014 Temperature 37.3 Celcius',
                                  'Single Point Mon Nov 24 13:20:44 2014 Temperature 37.5 Celcius'];
            while (table.next()) {
                const actualVal = table.record.Set + " " + table.record["Date Time"]+" "+table.record.Name + " "+table.record.Value + " " + table.record.Units;
                actualVal.should.equal(expectedVals[idx++]);
            }
            table.close();
            groupTarget.close();
            done();
        });

        after(function(done){
            file.close();
            done();
        });
    });

});

