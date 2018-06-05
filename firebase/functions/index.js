// The Cloud Functions for Firebase SDK to create Cloud Functions and setup triggers.
const functions = require('firebase-functions');


// This is a workaround where data is moved from input struct. It is needed
// as the SIM module can't do PUT requests.
exports.redirectData = functions.database.ref('input/{pushId}').onCreate((snapshot, context) => {

    const position = snapshot.child('pos').val();

    return snapshot.ref.root.child('current_pos').set((position), (error) => {
        if (error) {
            console.log('Data could not be written. I have no clue why.');
            snapshot.ref.remove();
        } else {
            console.log('Position updated successfully');
            snapshot.ref.remove();
        }
    });
});
