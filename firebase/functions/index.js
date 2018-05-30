// The Cloud Functions for Firebase SDK to create Cloud Functions and setup triggers.
const functions = require('firebase-functions');

// This is a workaround where data is moved from input struct. It is needed
// as the SIM module can't do PUT requests.
exports.redirectData = functions.database.ref('input/{pushId}').onCreate((snapshot, context) => {
    const original = snapshot.val();
    snapshot.ref.remove();
    console.log('Moving ', context.params.pushId, original);
    return snapshot.ref.parent.parent.child('Sigmatrucken').set((original), (error) => {
        if (error) {
            console.log('Data could not be written. I have no clue why.');
        } else {
            snapshot.ref.remove();
            console.log('Original data removed successfully');
        }
    });
});
